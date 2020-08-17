//
// Created by lyman on 2020/7/31.
//
#include "BasicFFMpeg.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include <stdio.h>
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/log.h"

#include <sys/stat.h>

#define INBUF_SIZE 4096


void my_log_output(void* ptr, int level, const char* fmt, va_list vl) {
    static int print_prefix = 1;
    static int count;
    static char prev[1024];
    char line[1024];
    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);
    strcpy(prev, line);
    if (level <= AV_LOG_WARNING)
    {
        LOGE(TAG_FFMPEG, "%s", line);
    }
    else
    {
        LOGD(TAG_FFMPEG, "%s", line);
    }
}


jint JNI_OnLoad(JavaVM* vm, void* reserved){
    LOGD(TAG, "JNI_Onload");
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    av_log_set_callback(my_log_output);  // 设置自定义的日志输出方法
    av_log(NULL, AV_LOG_INFO, "Hello World\n");

    result = JNI_VERSION_1_6;
    return result;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    LOGD(TAG, "JNI_OnUnload");
    av_log_set_callback(NULL);
}


JNIEXPORT jstring JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_getConfiguration
        (JNIEnv *env, jobject js) {
    char info[10000] = {0};
    LOGD(TAG, "getConfiguration");

    sprintf(info, "%s\n", avcodec_configuration());
    return env->NewStringUTF(info);
}


void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename,"w");
    //fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

void yuv_save(AVFrame* frame, char *filename)
{
    FILE *f;
    int y;

    f = fopen(filename,"w");
    for (y = 0; y < frame->height; y++)
        fwrite(&frame->data[0][y * frame->linesize[0]], 1, frame->width, f);
    for (y = 0; y < frame->height / 2; y++)
        fwrite(&frame->data[1][y * frame->linesize[1]], 1, frame->width / 2, f);
    for (y = 0; y < frame->height / 2; y++)
        fwrite(&frame->data[2][y * frame->linesize[2]], 1, frame->width / 2, f);

    fclose(f);
}


void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }


        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
        yuv_save(frame, buf);
    }
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_decodeVideoData2YUV420P
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring yuvDir) {
    jbyte *video_array = env->GetByteArrayElements(byteArray, 0);
    unsigned char *video_buffer = (unsigned char *)video_array;
    int video_length = env->GetArrayLength(byteArray);
    char *root_path = const_cast<char *>(env->GetStringUTFChars(rootOutputPath, 0));
    char *save_yuv_dir = const_cast<char *>(env->GetStringUTFChars(yuvDir, 0));
    char yuv_full_name[150];
    sprintf(yuv_full_name, "%s%s", save_yuv_dir, "/frame");
    // save file
    FILE *input_video_file;
    char input_video_file_path[120] = {0};
    sprintf(input_video_file_path, "%s%s", root_path, "/output.h264");
    if((input_video_file = fopen(input_video_file_path,"wb")) == NULL){
        LOGE(TAG, "decodeVideoData2YUV420P, failed to create output mp4 file path");
        return;
    }
    fwrite(video_buffer, 1, video_length, input_video_file);
    fclose(input_video_file);

    const char *filename, *outfilename;
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c= NULL;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t   data_size;
    int ret;
    AVPacket *pkt;

    filename    = input_video_file_path;
    outfilename = yuv_full_name;

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        LOGE(TAG, "decodeVideoData2YUV420P, Codec not found\n");
        return;
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        LOGE(TAG, "decodeVideoData2YUV420P, parser not found\n");
        return;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(TAG, "decodeVideoData2YUV420P, Could not allocate video codec context\n");
        return;
    }

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        LOGE(TAG, "decodeVideoData2YUV420P, Could not open codec\n");
        return;
    }

    f = fopen(filename, "rb");
    if (!f) {
        LOGE(TAG, "decodeVideoData2YUV420P, Could not open %s\n", filename);
        return;
    }

    frame = av_frame_alloc();

    if (!frame) {
        LOGE(TAG, "decodeVideoData2YUV420P, Could not allocate video frame\n");
        return;
    }

    while (!feof(f)) {
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (!data_size)
            break;

        /* use the parser to split the data into frames */
        data = inbuf;
        while (data_size > 0) {
            ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                   data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (ret < 0) {
                LOGE(TAG, "decodeVideoData2YUV420P, Error while parsing\n");
                return;
            }
            data      += ret;
            data_size -= ret;

            if (pkt->size)
                decode(c, frame, pkt, outfilename);
        }
    }

    /* flush the decoder */
    decode(c, frame, NULL, outfilename);

    fclose(f);
    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    env->ReleaseStringUTFChars(yuvDir, save_yuv_dir);
    env->ReleaseStringUTFChars(rootOutputPath, root_path);
    env->ReleaseByteArrayElements(byteArray, video_array, 0);
}

#ifdef __cplusplus
}
#endif
