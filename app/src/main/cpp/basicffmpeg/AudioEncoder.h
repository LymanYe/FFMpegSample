//
// Created by Lyman on 2020/8/20.
//

#ifndef FFMPEGSAMPLE_AUDIOENCODER_H
#define FFMPEGSAMPLE_AUDIOENCODER_H

#include "../base/Base.h"
extern "C" {
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/frame.h"
};

#define AUDIO_ENCODER_TAG "AudioEncoder"
class AudioEncoder {
private:
    const char *filename;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    AVFrame *frame;
    AVPacket *pkt;
    int i, j, k, ret;
    FILE *outputfile;
    uint16_t *samples;
    float t, tincr;

public:
    AudioEncoder(char *inputpath, char *outputpath, AVCodecID avCodecId);
    ~AudioEncoder();
    void encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt,
                FILE *output);
    void encodePCM2AAC();
    void destroy();
};


#endif //FFMPEGSAMPLE_AUDIOENCODER_H
