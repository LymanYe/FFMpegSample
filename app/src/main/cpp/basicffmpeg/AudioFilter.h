//
// Created by Lyman on 2020-08-22.
//

#ifndef FFMPEGSAMPLE_AUDIOFILTER_H
#define FFMPEGSAMPLE_AUDIOFILTER_H

extern "C" {
#include "../ffmpeg/libavutil/channel_layout.h"
#include "../ffmpeg/libavutil/md5.h"
#include "../ffmpeg/libavutil/mem.h"
#include "../ffmpeg/libavutil/opt.h"
#include "../ffmpeg/libavutil/samplefmt.h"
#include "../ffmpeg/libavutil/frame.h"
#include "../ffmpeg/libavfilter/avfilter.h"
#include "../ffmpeg/libavfilter/buffersink.h"
#include "../ffmpeg/libavfilter/buffersrc.h"
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavformat/avformat.h"
};

#include <../base/Base.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>


#define AUDIO_FILTER_TAG "AudioFilterTag"
#define INPUT_SAMPLERATE     48000
#define INPUT_FORMAT         AV_SAMPLE_FMT_FLTP
#define INPUT_CHANNEL_LAYOUT AV_CH_LAYOUT_5POINT0

#define FRAME_SIZE 1024
#define VOLUME_VAL 0.90

class AudioFilter {
private:
    struct AVMD5 *md5;
    AVFilterGraph *graph;
    AVFilterContext *src, *sink;
    AVFrame *frame;
    uint8_t errstr[1024];
    float duration;
    int err, nb_frames, i;
    int ret;


    // decode audio & send to filter
    AVPacket packet;
    AVFrame *filt_frame = av_frame_alloc();
    const char *filter_descr = "aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono";
    const char *player       = "ffplay -f s16le -ar 8000 -ac 1 -";
    AVFormatContext *fmt_ctx;
    AVCodecContext *dec_ctx;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    int audio_stream_index = -1;

public:
    void initSinWavePCMData(int duration);
    void filterSinWavePCMData();

    // decode audio & send to filter
    void decodeAudioAndInitFilter(char* inputfilename);
    void sendPCM2Filter();
    int open_input_file(const char *filename);
    int init_filters(const char *filters_descr);
};


#endif //FFMPEGSAMPLE_AUDIOFILTER_H
