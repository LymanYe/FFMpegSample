//
// Created by Lyman on 2020-09-18.
//

#ifndef FFMPEGSAMPLE_SWSCALE_H
#define FFMPEGSAMPLE_SWSCALE_H

#include <../base/Base.h>

extern "C" {
#include "../ffmpeg/libavutil/imgutils.h"
#include "../ffmpeg/libavutil/parseutils.h"
#include "../ffmpeg/libswscale/swscale.h"
};


#define SWSCALE_TAG "SWScale"

class SwScale {
private:
    uint8_t *src_data[4], *dst_data[4];
    int src_linesize[4], dst_linesize[4];
    int src_w = 320, src_h = 240, dst_w, dst_h;
    enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_YUV420P, dst_pix_fmt = AV_PIX_FMT_RGB24;

    const char *dst_size = NULL;
    const char *dst_filename = NULL;
    FILE *dst_file;
    int dst_bufsize;
    struct SwsContext *sws_ctx;
    int i, ret;

public:
    SwScale(char *outputfilepath);
    virtual ~SwScale();
    virtual void ConvertYUV420P2RGB();
};


#endif //FFMPEGSAMPLE_SWSCALE_H
