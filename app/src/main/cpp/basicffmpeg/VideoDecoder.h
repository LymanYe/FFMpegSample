//
// Created by Lyman on 2020/8/20.
//

#ifndef FFMPEGSAMPLE_VIDEODECODER_H
#define FFMPEGSAMPLE_VIDEODECODER_H

#include "../base/Base.h"
extern "C" {
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/frame.h"
};

#define INBUF_SIZE 4096
#define VIDEODECODER_TAG "VideoDecoderTag"

class VideoDecoder {
private:
    const char *inputfilename, *outfilename;
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c= NULL;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t   data_size;
    int ret;
    AVPacket *pkt;

public:
    VideoDecoder(char* inputpath, char* outputpath, AVCodecID avCodecId);
    virtual ~VideoDecoder();
    virtual void decodeVideo2YUV();
    virtual void decodeVideo(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                             const char *filename);
    virtual void destroy();
};


#endif //FFMPEGSAMPLE_VIDEODECODER_H
