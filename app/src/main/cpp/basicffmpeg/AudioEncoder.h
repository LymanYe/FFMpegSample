//
// Created by Lyman on 2020/8/20.
//

#ifndef FFMPEGSAMPLE_AUDIOENCODER_H
#define FFMPEGSAMPLE_AUDIOENCODER_H

#include "../base/Base.h"
extern "C" {
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/frame.h"
#include "../ffmpeg/libavformat/avformat.h"
};

#define AUDIO_ENCODER_TAG "AudioEncoder"
class AudioEncoder {
private:
    const char *filename;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    AVFormatContext *formatContext = NULL;
    AVFrame *frame;
    AVPacket *pkt;
    int i, j, k, ret;
    FILE *outputfile;
    FILE *inputfile;
    uint16_t *samples;
    float t, tincr;

public:
    AudioEncoder(char *inputpath, char *outputpath, AVCodecID avCodecId, int bitRate, AVSampleFormat sampleFmt);
    ~AudioEncoder();
    void encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt,
                FILE *output);
    void encodePCM2AAC();
    void encodePCM2MP3();
    void encodeSingleToneSound();
    void destroy();
};


#endif //FFMPEGSAMPLE_AUDIOENCODER_H
