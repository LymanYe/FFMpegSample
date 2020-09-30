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
#include "../ffmpeg/libavutil/mathematics.h"
};


#define MUXING_TAG "Muxing"


class Muxing {
private:
    AVFormatContext *audio_fmtCtx = NULL,*video_fmtCtx = NULL;
    AVFormatContext *out_fmtCtx = NULL;
    int audio_stream_index = -1;
    int video_stream_index = -1;
    int ret, video_frame_index = 0, audio_frame_index = 0;
    AVStream *audio_in_stream = NULL,*video_in_stream = NULL;
    AVStream *audio_ou_stream = NULL,*video_ou_stream = NULL;

    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    const char *in_filename, *out_filename;
    int i;
    int stream_index = 0;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
public:
    Muxing();
    virtual void initMuxing(char *inputfilepath_v, char *inputfilepath_a, char *outputpath);
    virtual void initReMuxing(char *inputfilepath, char *outputfilepath);
    virtual void doMuxing();
    virtual void doReMuxing();
    virtual void releaseReMuxingResource();
};


#endif //FFMPEGSAMPLE_MUXING_H
