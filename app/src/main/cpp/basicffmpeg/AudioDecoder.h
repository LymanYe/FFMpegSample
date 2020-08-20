//
// Created by Lyman on 2020/8/19.
//

#ifndef FFMPEGSAMPLE_AUDIODECODER_H
#define FFMPEGSAMPLE_AUDIODECODER_H

#include "../base/Base.h"

extern "C" {
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/log.h"
#include "../ffmpeg/libavutil/frame.h"
};

#define AUDIO_DECODER_TAG "AudioDecoder"
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 409

class AudioDecoder {
private:
    char *outfilename, *inputfilename;

    const AVCodec *codec;
    AVCodecContext *c = NULL;
    AVCodecParserContext *parser = NULL;
    FILE *inputfile, *outfile;
    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t   data_size;
    AVPacket *pkt;
    AVFrame *decoded_frame = NULL;

public:
    AudioDecoder(char* inputpath, char* outputpath, AVCodecID avCodecId);
    virtual ~AudioDecoder();
    virtual void decodeAudio2PCM();
    virtual void decodeAudio(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile);
    virtual void destroy();
};


#endif //FFMPEGSAMPLE_AUDIODECODER_H
