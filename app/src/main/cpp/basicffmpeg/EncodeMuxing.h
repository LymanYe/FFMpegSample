//
// Created by Lyman on 2020-09-30.
//

#ifndef FFMPEGSAMPLE_ENCODEMUXING_H
#define FFMPEGSAMPLE_ENCODEMUXING_H


#include <../base/Base.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern "C" {
#include "../ffmpeg/libavutil/avassert.h"
#include "../ffmpeg/libavutil/channel_layout.h"
#include "../ffmpeg/libavutil/opt.h"
#include "../ffmpeg/libavutil/mathematics.h"
#include "../ffmpeg/libavutil/timestamp.h"
#include "../ffmpeg/libavformat/avformat.h"
#include "../ffmpeg/libswscale/swscale.h"
#include "../ffmpeg/libswresample/swresample.h"
};

#define STREAM_DURATION   10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

#define SCALE_FLAGS SWS_BICUBIC
#define ENCODE_MUXING_TAG "EncodeMuxingTag"

// a wrapper around a single output AVStream
typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class EncodeMuxing {
private:
    OutputStream video_st = { 0 }, audio_st = { 0 };
    const char *filename;
    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVCodec *audio_codec, *video_codec;
    int ret;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;
    int i;

public:
    virtual void init(char* outputpath);
    virtual void encodeAndMuxer();
};


#endif //FFMPEGSAMPLE_ENCODEMUXING_H
