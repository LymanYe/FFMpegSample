//
// Created by Lyman on 2020/8/20.
//

#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(char *inputpath, char *outputpath, AVCodecID avCodecId) {
    inputfilename    = inputpath;
    outfilename = outputpath;
    pkt = av_packet_alloc();
    if (!pkt)
        return ;

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the H264 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, Codec not found\n");
        return;
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, parser not found\n");
        return;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, Could not allocate video codec context\n");
        return;
    }

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, Could not open codec\n");
        return;
    }

    f = fopen(inputfilename, "rb");
    if (!f) {
        LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, Could not open %s\n", inputfilename);
        return;
    }

    frame = av_frame_alloc();

    if (!frame) {
        LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, Could not allocate video frame\n");
        return;
    }
}


VideoDecoder::~VideoDecoder() {
    if(this->inputfilename != NULL) {
        delete[] inputfilename;
        inputfilename = NULL;
    }

    if(this->outfilename != NULL) {
        delete[] outfilename;
        outfilename = NULL;
    }
}


void VideoDecoder::decodeVideo2YUV() {
    while (!feof(f)) {
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (!data_size)
            break;

        /* use the parser to split the data into frames */
        data = inbuf;
        while (data_size > 0) {
            ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                   data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (ret < 0) {
                LOGE(VIDEODECODER_TAG, "decodeVideoData2YUV420P, Error while parsing\n");
                return;
            }
            data      += ret;
            data_size -= ret;

            if (pkt->size)
                decodeVideo(c, frame, pkt, outfilename);
        }
    }

    /* flush the decoder */
    decodeVideo(c, frame, NULL, outfilename);

    destroy();
}


void yuv_save(AVFrame* frame, char *filename)
{
    FILE *f;
    int y;

    f = fopen(filename,"w");
    for (y = 0; y < frame->height; y++)
        fwrite(&frame->data[0][y * frame->linesize[0]], 1, frame->width, f);
    for (y = 0; y < frame->height / 2; y++)
        fwrite(&frame->data[1][y * frame->linesize[1]], 1, frame->width / 2, f);
    for (y = 0; y < frame->height / 2; y++)
        fwrite(&frame->data[2][y * frame->linesize[2]], 1, frame->width / 2, f);

    fclose(f);
}


void VideoDecoder::decodeVideo(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                 const char *filename)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        LOGE(VIDEODECODER_TAG, "decode, Error sending a packet for decoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(VIDEODECODER_TAG, "decode, Error during decoding\n");
            return;
        }


        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
        yuv_save(frame, buf);
    }
}

void VideoDecoder::destroy() {
    fclose(f);
    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}