//
// Created by Lyman on 2020/8/20.
//

#include "VideoEncoder.h"

VideoEncoder::VideoEncoder(char *inputfilepath, char *outputfilepath, AVCodecID codecId) {
    filename = inputfilepath;
//    codec = avcodec_find_encoder_by_name(codec_name);
    codec = avcodec_find_encoder(codecId);
    if (!codec) {
        LOGE(VIDEO_ENCODE_TAG, "Codec '%d' not found\n", codecId);
        return ;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate video codec context\n");
        return ;
    }

    pkt = av_packet_alloc();
    if (!pkt)
        return ;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 352;
    c->height = 288;
    /* frames per second */
    c->time_base = (AVRational){1, 25};
    c->framerate = (AVRational){25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Could not open codec: %s\n", av_err2str(ret));
        return ;
    }

    if(inputfilepath != NULL) {
        inputFile = fopen(inputfilepath, "wb");
        if (!inputFile) {
            LOGE(VIDEO_ENCODE_TAG, "Could not open %s\n", inputfilepath);
            return ;
        }
    }

    if(outputfilepath != NULL) {
        outputFile = fopen(outputfilepath, "wb");
        if (!outputFile) {
            LOGE(VIDEO_ENCODE_TAG, "Could not open %s\n", outputfilepath);
            return ;
        }
    }


    frame = av_frame_alloc();
    if (!frame) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate video frame\n");
        return ;
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate the video frame data\n");
        return ;
    }
}


void VideoEncoder::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        LOGE(VIDEO_ENCODE_TAG, "Send frame %3" PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(VIDEO_ENCODE_TAG, "Error during encoding\n");
            return;
        }

        LOGD(VIDEO_ENCODE_TAG, "Write packet %3" PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}


void VideoEncoder::encodeGenerateYUV420P2H264() {
/* encode 1 second of video */
    for (i = 0; i < 25; i++) {
        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

        /* prepare a dummy image */
        /* Y */
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for (y = 0; y < c->height/2; y++) {
            for (x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, outputFile);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, outputFile);

    /* add sequence end code to have a real MPEG file */
//    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
//    fwrite(endcode, 1, sizeof(endcode), outputFile);
    fclose(outputFile);
}


VideoEncoder::~VideoEncoder() {

}
