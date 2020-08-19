//
// Created by lyman on 2020/7/31.
//
#include "BasicFFMpeg.h"
#include "AudioDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include <stdio.h>
#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/log.h"
#include "../ffmpeg/libavdevice/avdevice.h"

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


void decodeVideo(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                   const char *filename)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        LOGE(TAG, "decode, Error sending a packet for decoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(TAG, "decode, Error during decoding\n");
            return;
        }


        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
        yuv_save(frame, buf);
    }
}

// decode h264 to yuv420p
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
                decodeVideo(c, frame, pkt, outfilename);
        }
    }

    /* flush the decoder */
    decodeVideo(c, frame, NULL, outfilename);

    fclose(f);
    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    env->ReleaseStringUTFChars(yuvDir, save_yuv_dir);
    env->ReleaseStringUTFChars(rootOutputPath, root_path);
    env->ReleaseByteArrayElements(byteArray, video_array, 0);
}


// decode aac to pcm
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_decodeAudioData2PCM
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring aacDir) {
    jbyte *audio_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *audio_buffer = (unsigned char *)audio_array;
    int audio_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_pcm_dir = jstring2cchar(env, aacDir);
    char pcm_full_name[150];
    sprintf(pcm_full_name, "%s%s", save_pcm_dir, "/output.pcm");
    // save file
    FILE *input_audio_file;
    char input_audio_file_path[120] = {0};
    sprintf(input_audio_file_path, "%s%s", root_path, "/output.aac");
    if((input_audio_file = fopen(input_audio_file_path,"wb")) == NULL){
        LOGE(TAG, "decodeAudioData2PCM, failed to create output aac file path");
        return;
    }
    fwrite(audio_buffer, 1, audio_length, input_audio_file);
    fclose(input_audio_file);

    AudioDecoder *audioDecoder = new AudioDecoder(input_audio_file_path, pcm_full_name);
    audioDecoder->decodeAAC2PCM();
}

// 参考相关知识：https://blog.csdn.net/leixiaohua1020/article/details/39702113
// 找不到android_camera 这个input device 或者/dev/video0这个demuxer权限问题无法打开。没有实现...
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_openNativeCamera
        (JNIEnv *env, jobject js, jstring yuvDir) {
    char *save_yuv_dir = const_cast<char *>(env->GetStringUTFChars(yuvDir, 0));
    char yuv_full_name[150];
    sprintf(yuv_full_name, "%s%s", save_yuv_dir, "/output.yuv");

    int ret;
    AVFormatContext *fmtCtx = NULL;
    AVPacket pkt1, *pcaket = &pkt1;

    /*1、注册*/
    avdevice_register_all();
    /*2、连接视频源*/
    AVInputFormat *inputFmt = av_find_input_format("video4linux2");
    if (NULL != inputFmt) {
        LOGD(TAG, "openNativeCamera, input device name:%s\n", inputFmt->name);
    } else {
        LOGE(TAG, "openNativeCamera, Null point! for input format\n");
        return;
    }
#if 1
    AVDictionary *avdict = NULL;
    AVDictionaryEntry *t = av_dict_get(avdict, "video_size", NULL, AV_DICT_IGNORE_SUFFIX);
    av_dict_set(&avdict, "video_size", "hd720", 0);
    av_dict_set_int(&avdict, "camera_index",1, 0);
    av_dict_set_int(&avdict, "input_queue_size",2, 0);
    //if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
    //  printf("Could not find stream information\n");
    //}
    //printf("ok4\n");
    /*3、打开视频采集设备*/
    //ret = avformat_open_input(&fmtCtx, "/dev/video0", inputFmt, &avdict);
    //ret = avformat_open_input(&fmtCtx, "android_camera", inputFmt, &avdict);
    ret = avformat_open_input(&fmtCtx, NULL, inputFmt, &avdict);
    if(ret < 0)
    {
        LOGE(TAG, "openNativeCamera, av_demuxer_open error: %s\n", av_err2str(ret));
        return;
    }
    av_dict_free(&avdict);

#else
    fmtCtx = avformat_alloc_context();
    if(!fmtCtx)
    {
        printf("avformat_alloc_contest error\n");
        exit(1);
    }

#endif
    /*4、读取一帧数据，编码依据摄像头类型而定，我使用的摄像头输出的是yuv422格式*/
    fmtCtx->flags &= (~0x4);
    av_read_frame(fmtCtx, pcaket);
    /*5、写入帧数据到文件*/
    FILE *fp = NULL;
    fp = fopen(yuv_full_name, "a+");
    if (NULL != fp) {
        //将数据写入文件
        fwrite(pcaket->data, 1, pcaket->size, fp);
    }
    //关闭文件
    fclose(fp);
    /*6、释放读取的帧数据*/
    av_packet_free(&pcaket);
    /*7、关闭视频输入源*/
    avformat_close_input(&fmtCtx);

    env->ReleaseStringUTFChars(yuvDir, save_yuv_dir);
}

#ifdef __cplusplus
}
#endif
