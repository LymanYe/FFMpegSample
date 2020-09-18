//
// Created by Lyman on 2020-09-18.
//

#ifndef FFMPEGSAMPLE_RESAMPLE_H
#define FFMPEGSAMPLE_RESAMPLE_H

#include <../base/Base.h>

extern "C" {
#include "../ffmpeg/libavutil/opt.h"
#include "../ffmpeg/libavutil/channel_layout.h"
#include "../ffmpeg/libavutil/samplefmt.h"
#include "../ffmpeg/libswresample/swresample.h"
};


#define RESAMPLE_TAG "Resample"

class Resample {
private:
    int64_t src_ch_layout = AV_CH_LAYOUT_STEREO, dst_ch_layout = AV_CH_LAYOUT_SURROUND;
    int src_rate = 48000, dst_rate = 44100;
    uint8_t **src_data = NULL, **dst_data = NULL;
    int src_nb_channels = 0, dst_nb_channels = 0;
    int src_linesize, dst_linesize;
    int src_nb_samples = 1024, dst_nb_samples, max_dst_nb_samples;
    enum AVSampleFormat src_sample_fmt = AV_SAMPLE_FMT_DBL, dst_sample_fmt = AV_SAMPLE_FMT_S16;
    const char *dst_filename = NULL;
    FILE *dst_file;
    int dst_bufsize;
    const char *fmt;
    struct SwrContext *swr_ctx;
    double t;
    int ret;

public:
    Resample(char *outputpath);
    virtual void doResample();
    virtual ~Resample();
};


#endif //FFMPEGSAMPLE_RESAMPLE_H
