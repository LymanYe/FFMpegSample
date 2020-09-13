//
// Created by Lyman on 2020/8/20.
//

#include "AudioEncoder.h"

/* check that a given sample format is supported by the encoder */
int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

/* just pick the highest supported samplerate */
int select_sample_rate(const AVCodec *codec)
{
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    return best_samplerate;
}

/* select layout with the highest channel count */
int select_channel_layout(const AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

AudioEncoder::AudioEncoder(char *inputpath, char *outputpath, AVCodecID avCodecId, int bitRate, AVSampleFormat sampleFmt) {
    this->filename = outputpath;
    codec = avcodec_find_encoder(avCodecId);
    if (!codec) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Codec not found codeId: %d\n" , avCodecId);
        return;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Could not allocate audio codec context\n");
        return;
    }

    /* put sample parameters */
    c->bit_rate = bitRate;

    /* check that the encoder supports  pcm input */
    c->sample_fmt = sampleFmt;
    if (!check_sample_fmt(codec, c->sample_fmt)) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Encoder does not support sample format %s",
                av_get_sample_fmt_name(c->sample_fmt));
        return ;
    }

    /* select other audio parameters supported by the encoder */
    c->sample_rate    = select_sample_rate(codec);
    c->channel_layout = select_channel_layout(codec);
    c->channels       = av_get_channel_layout_nb_channels(c->channel_layout);

    /* Allow the use of the experimental AAC or MP3 encoder. */
    c->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Could not open codec\n");
        return ;
    }


    /* packet for holding encoded output */
    pkt = av_packet_alloc();
    if (!pkt) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, could not allocate the packet\n");
        return ;
    }

    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Could not allocate audio frame\n");
        return ;
    }

    frame->nb_samples     = c->frame_size;
    frame->format         = c->sample_fmt;
    frame->channel_layout = c->channel_layout;

    /* allocate the data buffers */
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Could not allocate audio data buffers\n");
        return ;
    }

    outputfile = fopen(outputpath, "wb");
    if (!outputfile) {
        LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Could not open %s\n", filename);
        return ;
    }

    if(inputpath != NULL) {
        inputfile = fopen(inputpath, "rb");
        if(!inputfile) {
            LOGE(AUDIO_ENCODER_TAG, "AudioEncoder, Could not open %s\n", inputpath);
            return ;
        }
    }

    if(avCodecId == AV_CODEC_ID_AAC) {
        formatContext = avformat_alloc_context();
        formatContext->oformat = av_guess_format(NULL, outputpath, NULL);
    }
}


AudioEncoder::~AudioEncoder() {
    if(filename != NULL) {
        delete[] filename;
        filename = NULL;
    }
}


void AudioEncoder::encodePCM2AAC() {
    LOGD(AUDIO_ENCODER_TAG,  "encodePCM2AAC");

    //Write Header
    //avformat_write_header(formatContext, NULL);


    int data_size;
    // sample_fmt = AV_SAMPLE_FMT_FLTP
    if(c->sample_fmt == AV_SAMPLE_FMT_FLTP) {
        data_size = av_get_bytes_per_sample(c->sample_fmt);
        while (feof(inputfile) == 0) {
            int i = 0, ch = 0;

            if ((ret = av_frame_make_writable(frame)) < 0)
            {
                LOGE(AUDIO_ENCODER_TAG,  "encodePCM2AAC, frame is not writable.\n");
                return;
            }

            for (i = 0; i < frame->nb_samples; i++)
            {
                for (ch = 0; ch < c->channels; ch++)
                {
                    fread(frame->data[ch] + data_size * i, 1, data_size, inputfile);
                }
            }

            encode(c, frame, pkt, outputfile);
        }
    } else if(c->sample_fmt == AV_SAMPLE_FMT_S16P){
        // sample_fmt = AV_SAMPLE_FMT_S16P
        data_size = av_samples_get_buffer_size(NULL, c->channels, c->frame_size, c->sample_fmt, 1);
        LOGD(AUDIO_ENCODER_TAG, "encodePCM2AAC, data_size = %d\n", data_size);
        while (feof(inputfile) == 0)
        {
            if ((ret = av_frame_make_writable(frame)) < 0)
            {
                LOGE(AUDIO_ENCODER_TAG,  "encodePCM2AAC, frame is not writable.\n");
                return;
            }

            fread(frame->data[0], 1, data_size, inputfile);
            encode(c, frame, pkt, outputfile);
        }
    }

    /* flush the encoder */
    encode(c, NULL, pkt, outputfile);

    //Write Trailer
    //av_write_trailer(formatContext);

    destroy();
}


void AudioEncoder::encodePCM2MP3() {
    LOGD(AUDIO_ENCODER_TAG,  "encodePCM2MP3");
    int data_size;
    // sample_fmt = AV_SAMPLE_FMT_FLTP
    if(c->sample_fmt == AV_SAMPLE_FMT_FLTP) {
        data_size = av_get_bytes_per_sample(c->sample_fmt);
        while (feof(inputfile) == 0) {
            int i = 0, ch = 0;

            if ((ret = av_frame_make_writable(frame)) < 0)
            {
                LOGE(AUDIO_ENCODER_TAG,  "encodePCM2MP3, frame is not writable.\n");
                return;
            }

            for (i = 0; i < frame->nb_samples; i++)
            {
                for (ch = 0; ch < c->channels; ch++)
                {
                    fread(frame->data[ch] + data_size * i, 1, data_size, inputfile);
                }
            }

            encode(c, frame, pkt, outputfile);
        }
    } else if(c->sample_fmt == AV_SAMPLE_FMT_S16P){
        // sample_fmt = AV_SAMPLE_FMT_S16P
        data_size = av_samples_get_buffer_size(NULL, c->channels, c->frame_size, c->sample_fmt, 1);
        LOGD(AUDIO_ENCODER_TAG, "encodePCM2MP3, data_size = %d\n", data_size);
        while (feof(inputfile) == 0)
        {
            if ((ret = av_frame_make_writable(frame)) < 0)
            {
                LOGE(AUDIO_ENCODER_TAG,  "encodePCM2MP3, frame is not writable.\n");
                return;
            }

            fread(frame->data[0], 1, data_size, inputfile);
            encode(c, frame, pkt, outputfile);
        }
    }


    /* flush the encoder */
    encode(c, NULL, pkt, outputfile);

    destroy();
}


void AudioEncoder::encodeSingleToneSound() {
/* encode a single tone sound */
    t = 0;
    tincr = 2 * M_PI * 440.0 / c->sample_rate;
    for (i = 0; i < 400; i++) {
        /* make sure the frame is writable -- makes a copy if the encoder
         * kept a reference internally */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);
        samples = (uint16_t*)frame->data[0];

        for (j = 0; j < c->frame_size; j++) {
            samples[2*j] = (int)(sin(t) * 10000);

            for (k = 1; k < c->channels; k++)
                samples[2*j + k] = samples[2*j];
            t += tincr;
        }
        encode(c, frame, pkt, outputfile);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, outputfile);

    destroy();
}


void AudioEncoder::encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, FILE *output) {
    int ret;

    /* send the frame for encoding */
    ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        LOGE(AUDIO_ENCODER_TAG, "encode, Error sending the frame to the encoder : %s\n", av_err2str(ret));
        return;
    }

    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0) {
        ret = avcodec_receive_packet(ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(AUDIO_ENCODER_TAG, "encode, Error encoding audio frame ret = %d \n", ret);
            return;
        }

        fwrite(pkt->data, 1, pkt->size, output);
        av_packet_unref(pkt);
    }
}


void AudioEncoder::destroy() {
    fclose(outputfile);

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&c);
}