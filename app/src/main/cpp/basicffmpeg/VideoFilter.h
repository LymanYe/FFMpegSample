//
// Created by Lyman on 2020-08-22.
//

#ifndef FFMPEGSAMPLE_VIDEOFILTER_H
#define FFMPEGSAMPLE_VIDEOFILTER_H

extern "C" {
#define __STDC_CONSTANT_MACROS
#include "../ffmpeg/libavfilter/avfilter.h"
#include "../ffmpeg/libavfilter/buffersink.h"
#include "../ffmpeg/libavfilter/buffersrc.h"
#include "../ffmpeg/libavutil/avutil.h"
#include "../ffmpeg/libavutil/imgutils.h"
#include "../ffmpeg/libavutil/frame.h"
};


#include "../base/Base.h"
#define VIDEO_FILTER_TAG "VideoFilterTag"

class VideoFilter {
private:

    int ret;
    FILE *fp_in, *fp_out;
    int in_width, in_height;
    AVFrame *frame_in;
    AVFrame *frame_out;
    unsigned char *frame_buffer_in;
    unsigned char *frame_buffer_out;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    int video_stream_index = -1;

public:
    void initSingleYUVFile(char *inputfilepath, char *outputfilepath, int width, int height);
    void initFilter();
    void filterFrame();
};


#endif //FFMPEGSAMPLE_VIDEOFILTER_H
