//
// Created by Lyman on 2020/8/20.
//

#ifndef FFMPEGSAMPLE_VIDEOENCODER_H
#define FFMPEGSAMPLE_VIDEOENCODER_H


#include "../base/Base.h"
extern "C" {
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/frame.h"
#include "../ffmpeg/libavutil/opt.h"
#include "../ffmpeg/libavutil/imgutils.h"
#include "../ffmpeg/libavformat/avformat.h"
};

#define VIDEO_ENCODE_TAG "VideoEncodeTag"
class VideoEncoder {
private:
    const char *filename, *codec_name;
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    AVStream *avStream;
    AVFormatContext *avFormatContext;
    int i, ret, x, y, width, height;
    FILE *inputFile, *outputFile;
    AVFrame *frame;
    AVPacket *pkt;


public:
    VideoEncoder(char *inputfilepath, char *outputfilepath, int width, int height, AVCodecID codecId);
    VideoEncoder(char *outputfilepath, AVCodecID codecId);
    ~VideoEncoder();
    void encodeGenerateYUV420P2H264();
    void encodeYUV420PImage2H264();
    void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                FILE *outfile);
    void encodeH264OrH265(AVFormatContext *fmtContext, AVCodecContext *cCtx, AVFrame *frame, AVPacket *pkt,
                FILE *outfile);
};


#endif //FFMPEGSAMPLE_VIDEOENCODER_H
