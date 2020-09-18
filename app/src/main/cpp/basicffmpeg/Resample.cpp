//
// Created by Lyman on 2020-09-18.
//

#include "Resample.h"

static int get_format_from_sample_fmt(const char **fmt,
                                      enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
            { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
            { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
            { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
            { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
            { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    LOGD(RESAMPLE_TAG,
            "Sample format %s not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return AVERROR(EINVAL);
}

/**
 * Fill dst buffer with nb_samples, generated starting from t.
 */
static void fill_samples(double *dst, int nb_samples, int nb_channels, int sample_rate, double *t)
{
    int i, j;
    double tincr = 1.0 / sample_rate, *dstp = dst;
    const double c = 2 * M_PI * 440.0;

    /* generate sin tone with 440Hz frequency and duplicated channels */
    for (i = 0; i < nb_samples; i++) {
        *dstp = sin(c * *t);
        for (j = 1; j < nb_channels; j++)
            dstp[j] = dstp[0];
        dstp += nb_channels;
        *t += tincr;
    }
}

Resample::Resample(char *outputpath) {
    LOGD(RESAMPLE_TAG, "Resample, output path = %s", outputpath);
    dst_filename = outputpath;
    //dst_filename = "/storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/SWSRESAMPLE/ouput_s16.pcm";

    dst_file = fopen(dst_filename, "wb");
    if (!dst_file) {
        LOGE(RESAMPLE_TAG, "Could not open destination file %s\n", dst_filename);
        return ;
    }

    /* create resampler context */
    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        LOGE(RESAMPLE_TAG, "Could not allocate resampler context\n");
        ret = AVERROR(ENOMEM);
        return ;
    }

    /* set options */
    av_opt_set_int(swr_ctx, "in_channel_layout",    src_ch_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate",       src_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", src_sample_fmt, 0);

    av_opt_set_int(swr_ctx, "out_channel_layout",    dst_ch_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate",       dst_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dst_sample_fmt, 0);

    /* initialize the resampling context */
    if ((ret = swr_init(swr_ctx)) < 0) {
        LOGE(RESAMPLE_TAG, "Failed to initialize the resampling context\n");
        return ;
    }

    /* allocate source and destination samples buffers */

    src_nb_channels = av_get_channel_layout_nb_channels(src_ch_layout);
    ret = av_samples_alloc_array_and_samples(&src_data, &src_linesize, src_nb_channels,
                                             src_nb_samples, src_sample_fmt, 0);
    if (ret < 0) {
        LOGE(RESAMPLE_TAG, "Could not allocate source samples\n");
        return ;
    }

    /* compute the number of converted samples: buffering is avoided
     * ensuring that the output buffer will contain at least all the
     * converted input samples */
    max_dst_nb_samples = dst_nb_samples =
            av_rescale_rnd(src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);

    /* buffer is going to be directly written to a rawaudio file, no alignment */
    dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
    ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
                                             dst_nb_samples, dst_sample_fmt, 0);
    if (ret < 0) {
        LOGE(RESAMPLE_TAG, "Could not allocate destination samples\n");
        return ;
    }
}


void Resample::doResample() {
    t = 0;
    do {
        /* generate synthetic audio */
        fill_samples((double *)src_data[0], src_nb_samples, src_nb_channels, src_rate, &t);

        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, src_rate) +
                                        src_nb_samples, dst_rate, src_rate, AV_ROUND_UP);
        if (dst_nb_samples > max_dst_nb_samples) {
            av_freep(&dst_data[0]);
            ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
                                   dst_nb_samples, dst_sample_fmt, 1);
            if (ret < 0)
                break;
            max_dst_nb_samples = dst_nb_samples;
        }

        /* convert to destination format */
        ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t **)src_data, src_nb_samples);
        if (ret < 0) {
            LOGE(RESAMPLE_TAG, "Error while converting\n");
            goto end;
        }
        dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
                                                 ret, dst_sample_fmt, 1);
        if (dst_bufsize < 0) {
            LOGE(RESAMPLE_TAG, "Could not get sample buffer size\n");
            goto end;
        }
        LOGD(RESAMPLE_TAG, "t:%f in:%d out:%d\n", t, src_nb_samples, ret);
        fwrite(dst_data[0], 1, dst_bufsize, dst_file);
    } while (t < 10);

    if ((ret = get_format_from_sample_fmt(&fmt, dst_sample_fmt)) < 0)
        goto end;
    LOGD(RESAMPLE_TAG, "Resampling succeeded. Play the output file with the command:\n"
                    "ffplay -f %s -channel_layout %" PRId64" -channels %d -ar %d %s\n",
            fmt, dst_ch_layout, dst_nb_channels, dst_rate, dst_filename);


    end:
    fclose(dst_file);

    if (src_data)
        av_freep(&src_data[0]);
    av_freep(&src_data);

    if (dst_data)
        av_freep(&dst_data[0]);
    av_freep(&dst_data);

    swr_free(&swr_ctx);
}


Resample::~Resample() {

}