//
// Created by Lyman on 2020-09-19.
//

#ifndef FFMPEGSAMPLE_DEMUXING_H
#define FFMPEGSAMPLE_DEMUXING_H


#include <../base/Base.h>

extern "C" {
#include "../ffmpeg/libavformat/avformat.h"
#include "../ffmpeg/libavutil/avutil.h"
#include "../ffmpeg/libavcodec/avcodec.h"
};


#define DEMUXING_TAG "Demuxing"
//'1': Use H.264 Bitstream Filter
#define USE_H264BSF 1

class DeMuxing {
private:
    AVFormatContext *formatContext = NULL;
    AVPacket pkt;
    int ret, i;
    int videoindex=-1,audioindex=-1;
    const char *inputfilename;//Input file URL
    const char *outputfilename_v;//Output file URL
    const char *outputfilename_a;
    FILE *fp_audio, *fp_video;

public:
    DeMuxing(char *inputfilepath, char *outputDir);
    virtual ~DeMuxing();
    virtual void demuxingSampleVideo();
};


#endif //FFMPEGSAMPLE_DEMUXING_H
