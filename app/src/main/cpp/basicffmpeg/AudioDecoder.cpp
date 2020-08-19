//
// Created by Lyman on 2020/8/19.
//

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder(char *inputpath, char *outputpath) {
    this->inputfilename = inputpath;
    this->outfilename = outputpath;

    pkt = av_packet_alloc();

    codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
    if (!codec) {
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, Codec not found\n");
        return;
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, Parser not found\n");
        return;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, Could not allocate audio codec context\n");
        return;
    }

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, Could not open codec\n");
        return;
    }

    /** for dump format info
    //分配一个avformat
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    if (fmt_ctx == NULL)
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, alloc AVFormatContext fail");

    //打开文件，解封装
    if (avformat_open_input(&fmt_ctx, inputpath, NULL, NULL) != 0)
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, open input format failed");

    //查找文件的相关流信息
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0)
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, find stream info failed");


    //输出格式信息
    LOGD(AUDIO_DECODER_TAG, "AudioDecoder, start dump format");
    av_dump_format(fmt_ctx, 0, inputpath, 0);
     */

    inputfile = fopen(inputfilename, "rb");
    if (!inputfile) {
        LOGE(AUDIO_DECODER_TAG, "AudioDecoder, Could not open %s\n", inputfilename);
        return;
    }
    outfile = fopen(outfilename, "wb");
    if (!outfile) {
        av_free(c);
        return;
    }
}


AudioDecoder::~AudioDecoder() {
    if(this->inputfilename != NULL) {
        delete[] inputfilename;
        inputfilename = NULL;
    }

    if(this->outfilename != NULL) {
        delete[] outfilename;
        outfilename = NULL;
    }
}


void AudioDecoder::decodeAAC2PCM() {
    data      = inbuf;
    data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, inputfile);

    int ret, len;
    while (data_size > 0) {
        if (!decoded_frame) {
            if (!(decoded_frame = av_frame_alloc())) {
                LOGE(AUDIO_DECODER_TAG, "decodeAAC2PCM, Could not allocate audio frame\n");
                return;
            }
        }

        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                               data, data_size,
                               AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            LOGE(AUDIO_DECODER_TAG, "decodeAAC2PCM, Error while parsing\n");
            return;
        }
        data      += ret;
        data_size -= ret;

        if (pkt->size)
            decodeAudio(c, pkt, decoded_frame, outfile);

        if (data_size < AUDIO_REFILL_THRESH) {
            memmove(inbuf, data, data_size);
            data = inbuf;
            len = fread(data + data_size, 1,
                        AUDIO_INBUF_SIZE - data_size, inputfile);
            if (len > 0)
                data_size += len;
        }
    }

    destroy();
}


void convertAV_SAMPLE_FMT_FLTP_TO_S16P(AVFrame *audioFrame, FILE* file){
    int in_samples = audioFrame->nb_samples;
    short *outputBuffer = static_cast<short *>(malloc(audioFrame->nb_samples * 2 * 2));
    int i=0;
    float* inputChannel0 = (float*)audioFrame->extended_data[0];
    LOGD(AUDIO_DECODER_TAG, "convertAV_SAMPLE_FMT_FLTP_TO_S16P, channels= %d", audioFrame->channels);
    // Mono
    if (audioFrame->channels == 1) {
        for (i = 0 ; i < in_samples ; i++) {
            float sample = *inputChannel0++;
            if (sample < -1.0f)
                sample = -1.0f;
            else if (sample > 1.0f)
                sample = 1.0f;
            outputBuffer[i] = (int16_t) (sample * 32767.0f);
        }
    }
    //Stereo fltp planer转换为S16P
    else {
        float* inputChannel1 = (float*)audioFrame->extended_data[1];
        for (i=0 ; i< in_samples ; i++) {
            outputBuffer[i*2] = (int16_t) ((*inputChannel0++) * 32767.0f);
            outputBuffer[i*2+1] = (int16_t) ((*inputChannel1++) * 32767.0f);
        }
    }

    char data[1024 * 1024];
    memset(data, 0 , 1024 * 1024);
    if(file != NULL) {
        fwrite(outputBuffer, 2, in_samples * 2, file);
    }
}

void AudioDecoder::decodeAudio(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile)
{
    int ret, data_size;

    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        LOGE(AUDIO_DECODER_TAG,"decodeAudio, Error submitting the packet to the decoder\n");
        return;
    }

    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(AUDIO_DECODER_TAG,"decodeAudio, Error during decoding\n");
            return;
        }
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            LOGE(AUDIO_DECODER_TAG,"decodeAudio, Failed to calculate data size\n");
            return;
        }

        LOGD(AUDIO_DECODER_TAG, "decodeAudio, nb_samples: %d, channels: %d, sample_rate: %d, data_size: %d, sample_fmt: %d, AV_SAMPLE_FMT_S16: %d, sample_fmt == AV_SAMPLE_FMT_S16 -> %d", frame->nb_samples,
             dec_ctx->channels, dec_ctx->sample_rate, data_size, dec_ctx->sample_fmt, AV_SAMPLE_FMT_S16, dec_ctx->sample_fmt == AV_SAMPLE_FMT_S16);

        // FFMpeg默认使用AV_SAMPLE_FMT_FLTP解码采样并输出planer格式的数据，所以我们做下面的转换
        // 知识点参考：https://blog.csdn.net/shaosunrise/article/details/76571969
        // 使用命令：ffmpeg -f s16le -ar 41400 -ac 2 -i output.pcm output.aac看我们解出来的pcm编码后能不能播放（s16le对应S16P采样，也可以使用FLTP采样保存但是注意编码aac时候的格式）
        convertAV_SAMPLE_FMT_FLTP_TO_S16P(frame, outfile);
    }
}


void AudioDecoder::destroy() {
    fclose(outfile);
    fclose(inputfile);
    avcodec_free_context(&c);
    av_parser_close(parser);
    av_frame_free(&decoded_frame);
    av_packet_free(&pkt);
}
