//
// Created by Lyman on 2020-09-21.
//

#ifndef FFMPEGSAMPLE_MUXING_H
#define FFMPEGSAMPLE_MUXING_H


#include <../base/Base.h>

extern "C" {
#include "../ffmpeg/libavformat/avformat.h"
#include "../ffmpeg/libavutil/avutil.h"
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/timestamp.h"
};


#define MUXING_TAG "Muxing"
#define MUXING_USE_H264BSF 1
#define MUXING_USE_AACBSF 0


class Muxing {
private:
    AVFormatContext *audio_fmtCtx = NULL,*video_fmtCtx = NULL;
    AVFormatContext *out_fmtCtx = NULL;
    int audio_stream_index = -1;
    int video_stream_index = -1;
    int ret, video_frame_index = 0, audio_frame_index = 0;
    AVStream *audio_in_stream = NULL,*video_in_stream = NULL;
    AVStream *audio_ou_stream = NULL,*video_ou_stream = NULL;

public:
    Muxing();
    virtual void initMuxing(char *inputfilepath_v, char *inputfilepath_a, char *outputpath);
    virtual void doMuxing();
};


#endif //FFMPEGSAMPLE_MUXING_H
