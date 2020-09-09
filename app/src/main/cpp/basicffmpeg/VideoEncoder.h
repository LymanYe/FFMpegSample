//
// Created by Lyman on 2020/8/20.
//

#ifndef FFMPEGSAMPLE_VIDEOENCODER_H
#define FFMPEGSAMPLE_VIDEOENCODER_H

#include "../base/Base.h"
extern "C" {
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/frame.h"
#include "../ffmpeg/libavutil/opt.h"
};

#define VIDEO_ENCODE_TAG "VideoEncodeTag"
class VideoEncoder {
private:
    const char *filename, *codec_name;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;


public:
    VideoEncoder(char *inputfilepath, char *codecname);
    ~VideoEncoder();
    void encodeYUV2H264();
    void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                FILE *outfile);
};


#endif //FFMPEGSAMPLE_VIDEOENCODER_H
