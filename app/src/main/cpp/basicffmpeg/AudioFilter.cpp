//
// Created by Lyman on 2020-08-22.
//

#include "AudioFilter.h"

static int init_filter_graph(AVFilterGraph **graph, AVFilterContext **src,
                             AVFilterContext **sink)
{
    AVFilterGraph *filter_graph;
    AVFilterContext *abuffer_ctx;
    const AVFilter  *abuffer;
    AVFilterContext *volume_ctx;
    const AVFilter  *volume;
    AVFilterContext *aformat_ctx;
    const AVFilter  *aformat;
    AVFilterContext *abuffersink_ctx;
    const AVFilter  *abuffersink;

    AVDictionary *options_dict = NULL;
    uint8_t options_str[1024];
    uint8_t ch_layout[64];

    int err;

    /* Create a new filtergraph, which will contain all the filters. */
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Unable to create filter graph.\n");
        return AVERROR(ENOMEM);
    }

    /* Create the abuffer filter;
     * it will be used for feeding the data into the graph. */
    abuffer = avfilter_get_by_name("abuffer");
    if (!abuffer) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not find the abuffer filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffer_ctx = avfilter_graph_alloc_filter(filter_graph, abuffer, "src");
    if (!abuffer_ctx) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not allocate the abuffer instance.\n");
        return AVERROR(ENOMEM);
    }

    /* Set the filter options through the AVOptions API. */
    av_get_channel_layout_string(reinterpret_cast<char *>(ch_layout), sizeof(ch_layout), 0, INPUT_CHANNEL_LAYOUT);
    av_opt_set    (abuffer_ctx, "channel_layout", reinterpret_cast<const char *>(ch_layout), AV_OPT_SEARCH_CHILDREN);
    av_opt_set    (abuffer_ctx, "sample_fmt",     av_get_sample_fmt_name(INPUT_FORMAT), AV_OPT_SEARCH_CHILDREN);
    av_opt_set_q  (abuffer_ctx, "time_base",      (AVRational){ 1, INPUT_SAMPLERATE },  AV_OPT_SEARCH_CHILDREN);
    av_opt_set_int(abuffer_ctx, "sample_rate",    INPUT_SAMPLERATE,                     AV_OPT_SEARCH_CHILDREN);

    /* Now initialize the filter; we pass NULL options, since we have already
     * set all the options above. */
    err = avfilter_init_str(abuffer_ctx, NULL);
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not initialize the abuffer filter.\n");
        return err;
    }

    /* Create volume filter. */
    volume = avfilter_get_by_name("volume");
    if (!volume) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not find the volume filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    volume_ctx = avfilter_graph_alloc_filter(filter_graph, volume, "volume");
    if (!volume_ctx) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not allocate the volume instance.\n");
        return AVERROR(ENOMEM);
    }

    /* A different way of passing the options is as key/value pairs in a
     * dictionary. */
    av_dict_set(&options_dict, "volume", AV_STRINGIFY(VOLUME_VAL), 0);
    err = avfilter_init_dict(volume_ctx, &options_dict);
    av_dict_free(&options_dict);
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not initialize the volume filter.\n");
        return err;
    }

    /* Create the aformat filter;
     * it ensures that the output is of the format we want. */
    aformat = avfilter_get_by_name("aformat");
    if (!aformat) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not find the aformat filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    aformat_ctx = avfilter_graph_alloc_filter(filter_graph, aformat, "aformat");
    if (!aformat_ctx) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not allocate the aformat instance.\n");
        return AVERROR(ENOMEM);
    }

    /* A third way of passing the options is in a string of the form
     * key1=value1:key2=value2.... */
    snprintf(reinterpret_cast<char *>(options_str), sizeof(options_str),
             "sample_fmts=%s:sample_rates=%d:channel_layouts=0x%"PRIx64,
             av_get_sample_fmt_name(AV_SAMPLE_FMT_S16), 44100,
             (uint64_t)AV_CH_LAYOUT_STEREO);
    err = avfilter_init_str(aformat_ctx, reinterpret_cast<const char *>(options_str));
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not initialize the aformat filter.\n");
        return err;
    }

    /* Finally create the abuffersink filter;
     * it will be used to get the filtered data out of the graph. */
    abuffersink = avfilter_get_by_name("abuffersink");
    if (!abuffersink) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not find the abuffersink filter.\n");
        return AVERROR_FILTER_NOT_FOUND;
    }

    abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");
    if (!abuffersink_ctx) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not allocate the abuffersink instance.\n");
        return AVERROR(ENOMEM);
    }

    /* This filter takes no options. */
    err = avfilter_init_str(abuffersink_ctx, NULL);
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Could not initialize the abuffersink instance.\n");
        return err;
    }

    /* Connect the filters;
     * in this simple case the filters just form a linear chain. */
    err = avfilter_link(abuffer_ctx, 0, volume_ctx, 0);
    if (err >= 0)
        err = avfilter_link(volume_ctx, 0, aformat_ctx, 0);
    if (err >= 0)
        err = avfilter_link(aformat_ctx, 0, abuffersink_ctx, 0);
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Error connecting filters\n");
        return err;
    }

    /* Configure the graph. */
    err = avfilter_graph_config(filter_graph, NULL);
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filter_graph, Error configuring the filter graph\n");
        return err;
    }

    *graph = filter_graph;
    *src   = abuffer_ctx;
    *sink  = abuffersink_ctx;

    return 0;
}

/* Do something useful with the filtered data: this simple
 * example just prints the MD5 checksum of each plane to stdout. */
static int process_output(struct AVMD5 *md5, AVFrame *frame)
{
    int planar     = av_sample_fmt_is_planar(static_cast<AVSampleFormat>(frame->format));
    int channels   = av_get_channel_layout_nb_channels(frame->channel_layout);
    int planes     = planar ? channels : 1;
    int bps        = av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format));
    int plane_size = bps * frame->nb_samples * (planar ? 1 : channels);
    int i, j;

    for (i = 0; i < planes; i++) {
        uint8_t checksum[16];

        av_md5_init(md5);
        av_md5_sum(checksum, frame->extended_data[i], plane_size);

        LOGE(AUDIO_FILTER_TAG, "process_output, plane %d:", i);
        for (j = 0; j < sizeof(checksum); j++)
            LOGE(AUDIO_FILTER_TAG, "process_output, %02X", checksum[j]);
    }
    LOGE(AUDIO_FILTER_TAG, "\n");

    return 0;
}

/* Construct a frame of audio data to be filtered;
 * this simple example just synthesizes a sine wave. */
static int get_input(AVFrame *frame, int frame_num)
{
    int err, i, j;

    /* Set up the frame properties and allocate the buffer for the data. */
    frame->sample_rate    = INPUT_SAMPLERATE;
    frame->format         = INPUT_FORMAT;
    frame->channel_layout = INPUT_CHANNEL_LAYOUT;
    frame->nb_samples     = FRAME_SIZE;
    frame->pts            = frame_num * FRAME_SIZE;

    err = av_frame_get_buffer(frame, 0);
    if (err < 0)
        return err;

    /* Fill the data for each channel. */
    for (i = 0; i < 5; i++) {
        float *data = (float*)frame->extended_data[i];

        for (j = 0; j < frame->nb_samples; j++)
            data[j] = sin(2 * M_PI * (frame_num + j) * (i + 1) / FRAME_SIZE);
    }

    return 0;
}


void AudioFilter::initSinWavePCMData(int duration) {
    this->duration = duration;
    nb_frames = duration * INPUT_SAMPLERATE / FRAME_SIZE;
    if (nb_frames <= 0) {
        LOGE(AUDIO_FILTER_TAG, "initSinWavePCMData, Invalid duration: %s\n", duration);
        return;
    }

    /* Allocate the frame we will be using to store the data. */
    frame  = av_frame_alloc();
    if (!frame) {
        LOGE(AUDIO_FILTER_TAG, "initSinWavePCMData, Error allocating the frame\n");
        return;
    }

    md5 = av_md5_alloc();
    if (!md5) {
        LOGE(AUDIO_FILTER_TAG, "initSinWavePCMData, Error allocating the MD5 context\n");
        return;
    }

    /* Set up the filtergraph. */
    err = init_filter_graph(&graph, &src, &sink);
    if (err < 0) {
        LOGE(AUDIO_FILTER_TAG, "initSinWavePCMData, Unable to init filter graph:");
        return;
    }
}



void AudioFilter::filterSinWavePCMData() {
/* the main filtering loop */
    for (i = 0; i < nb_frames; i++) {
        /* get an input frame to be filtered */
        err = get_input(frame, i);
        if (err < 0) {
            LOGE(AUDIO_FILTER_TAG, "filterSinWavePCMData, Error generating input frame:");
            return;
        }

        /* Send the frame to the input of the filtergraph. */
        err = av_buffersrc_add_frame(src, frame);
        if (err < 0) {
            av_frame_unref(frame);
            LOGE(AUDIO_FILTER_TAG, "filterSinWavePCMData, Error submitting the frame to the filtergraph:");
            return;
        }

        /* Get all the filtered output that is available. */
        while ((err = av_buffersink_get_frame(sink, frame)) >= 0) {
            /* now do something with our filtered frame */
            err = process_output(md5, frame);
            if (err < 0) {
                LOGE(AUDIO_FILTER_TAG, "filterSinWavePCMData, Error processing the filtered frame:");
                return;
            }
            av_frame_unref(frame);
        }

        if (err == AVERROR(EAGAIN)) {
            /* Need to feed more frames in. */
            continue;
        } else if (err == AVERROR_EOF) {
            /* Nothing more to do, finish. */
            break;
        } else if (err < 0) {
            /* An error occurred. */
            LOGE(AUDIO_FILTER_TAG, "filterSinWavePCMData, Error filtering the data:");
            return;
        }
    }

    avfilter_graph_free(&graph);
    av_frame_free(&frame);
    av_freep(&md5);
}


int AudioFilter::open_input_file(const char *filename)
{
    int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        LOGE(AUDIO_FILTER_TAG, "open_input_file, Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        LOGE(AUDIO_FILTER_TAG, "open_input_file, Cannot find stream information\n");
        return ret;
    }

    /* select the audio stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0) {
        LOGE(AUDIO_FILTER_TAG, "open_input_file, Cannot find an audio stream in the input file\n");
        return ret;
    }
    audio_stream_index = ret;

    /* create decoding context */
    dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[audio_stream_index]->codecpar);

    /* init the audio decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        LOGE(AUDIO_FILTER_TAG, "open_input_file, Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}

int AudioFilter::init_filters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    const AVFilter *abuffersrc  = avfilter_get_by_name("abuffer");
    const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    static const enum AVSampleFormat out_sample_fmts[] = {AV_SAMPLE_FMT_S16,
                                                          static_cast<const AVSampleFormat>(-1)};
    static const int64_t out_channel_layouts[] = { AV_CH_LAYOUT_MONO, -1 };
    static const int out_sample_rates[] = { 8000, -1 };
    const AVFilterLink *outlink;
    AVRational time_base = fmt_ctx->streams[audio_stream_index]->time_base;

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx->channel_layout)
        dec_ctx->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
    snprintf(args, sizeof(args),
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
             time_base.num, time_base.den, dec_ctx->sample_rate,
             av_get_sample_fmt_name(dec_ctx->sample_fmt), dec_ctx->channel_layout);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, abuffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filters, Cannot create audio buffer source\n");
        goto end;
    }

    /* buffer audio sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filters, Cannot create audio buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filters, Cannot set output sample format\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "channel_layouts", out_channel_layouts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filters, Cannot set output channel layout\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE(AUDIO_FILTER_TAG, "init_filters, Cannot set output sample rate\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    /* Print summary of the sink buffer
     * Note: args buffer is reused to store channel layout string */
    outlink = buffersink_ctx->inputs[0];
    av_get_channel_layout_string(args, sizeof(args), -1, outlink->channel_layout);
    LOGD(AUDIO_FILTER_TAG, "init_filters, Output: srate:%dHz fmt:%s chlayout:%s\n",
           (int)outlink->sample_rate,
           (char *)av_x_if_null(av_get_sample_fmt_name((AVSampleFormat)outlink->format), "?"),
           args);

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

static void print_frame(const AVFrame *frame)
{
    LOGD(AUDIO_FILTER_TAG, "print frame");
    const int n = frame->nb_samples * av_get_channel_layout_nb_channels(frame->channel_layout);
    const uint16_t *p     = (uint16_t*)frame->data[0];
    const uint16_t *p_end = p + n;

    while (p < p_end) {
        fputc(*p    & 0xff, stdout);
        fputc(*p>>8 & 0xff, stdout);
        p++;
    }
    fflush(stdout);
}


void AudioFilter::decodeAudioAndInitFilter(char *inputfilename) {
    if ((ret = open_input_file(inputfilename) < 0))
        return;
    if ((ret = init_filters(filter_descr)) < 0)
        return;
}


void AudioFilter::sendPCM2Filter() {
/* read all packets */
    while (1) {
        LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, read frame");
        if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
            break;
        LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, read to packet");
        if (packet.stream_index == audio_stream_index) {
            LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, send packet");
            ret = avcodec_send_packet(dec_ctx, &packet);
            if (ret < 0) {
                LOGE(AUDIO_FILTER_TAG, "sendPCM2Filter, Error while sending a packet to the decoder\n");
                break;
            }

            while (ret >= 0) {
                LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, start receive frame");
                ret = avcodec_receive_frame(dec_ctx, frame);
                LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, end receive frame");
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    LOGE(AUDIO_FILTER_TAG, "sendPCM2Filter, Error while receiving a frame from the decoder\n");
                    goto end;
                }

                if (ret >= 0) {
                    /* push the audio data from decoded frame into the filtergraph */
                    LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, push the audio data from decoded frame into the filtergraph");
                    if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                        LOGE(AUDIO_FILTER_TAG, "sendPCM2Filter, Error while feeding the audio filtergraph\n");
                        break;
                    }

                    /* pull filtered audio from the filtergraph */
                    while (1) {
                        LOGD(AUDIO_FILTER_TAG, "sendPCM2Filter, av_buffersink_get_frame");
                        ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                            break;
                        if (ret < 0)
                            goto end;
                        print_frame(filt_frame);
                        av_frame_unref(filt_frame);
                    }
                    av_frame_unref(frame);
                }
            }
        }
        av_packet_unref(&packet);
    }
    end:
    avfilter_graph_free(&filter_graph);
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);

    if (ret < 0 && ret != AVERROR_EOF) {
        LOGE(AUDIO_FILTER_TAG, "sendPCM2Filter, Error occurred: %s\n", av_err2str(ret));
        return;
    }
}