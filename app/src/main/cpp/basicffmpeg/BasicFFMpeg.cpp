//
// Created by lyman on 2020/7/31.
//
#include "BasicFFMpeg.h"
#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "AudioEncoder.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../ffmpeg/libavcodec/avcodec.h"
#include "../ffmpeg/libavutil/log.h"
#include "../ffmpeg/libavdevice/avdevice.h"
#include <sys/stat.h>


void custom_log_output(void* ptr, int level, const char* fmt, va_list vl) {
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

    av_log_set_callback(custom_log_output);  // 设置自定义的日志输出方法
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
    jbyte *video_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *video_buffer = (unsigned char *)video_array;
    int video_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_yuv_dir = jstring2cchar(env, yuvDir);
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

    VideoDecoder *videoDecoder = new VideoDecoder(input_video_file_path, yuv_full_name, AV_CODEC_ID_H264);
    videoDecoder->decodeVideo2YUV();
}


// decode aac to pcm
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_decodeAACData2PCM
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring aacDir) {
    jbyte *audio_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *audio_buffer = (unsigned char *)audio_array;
    int audio_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_pcm_dir = jstring2cchar(env, aacDir);
    char pcm_full_name[150];
    sprintf(pcm_full_name, "%s%s", save_pcm_dir, "/aac_output.pcm");
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

    AudioDecoder *audioDecoder = new AudioDecoder(input_audio_file_path, pcm_full_name, AV_CODEC_ID_AAC);
    audioDecoder->decodeAudio2PCM();
}


// decode mp3 to pcm
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_decodeMP3Data2PCM
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring aacDir) {
    jbyte *audio_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *audio_buffer = (unsigned char *)audio_array;
    int audio_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_pcm_dir = jstring2cchar(env, aacDir);
    char pcm_full_name[150];
    sprintf(pcm_full_name, "%s%s", save_pcm_dir, "/mp3_output.pcm");
    // save file
    FILE *input_audio_file;
    char input_audio_file_path[120] = {0};
    sprintf(input_audio_file_path, "%s%s", root_path, "/output.mp3");
    if((input_audio_file = fopen(input_audio_file_path,"wb")) == NULL){
        LOGE(TAG, "decodeAudioData2PCM, failed to create output aac file path");
        return;
    }
    fwrite(audio_buffer, 1, audio_length, input_audio_file);
    fclose(input_audio_file);

    AudioDecoder *audioDecoder = new AudioDecoder(input_audio_file_path, pcm_full_name, AV_CODEC_ID_MP3);
    audioDecoder->decodeAudio2PCM();
}


// encode pcm s16le to AAC
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encodePCMData2AAC
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring aacDir) {
    jbyte *pcm_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *audio_buffer = (unsigned char *)pcm_array;
    int pcm_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_aac_dir = jstring2cchar(env, aacDir);
    char aac_full_name[150];
    sprintf(aac_full_name, "%s%s", save_aac_dir, "/encoded_output.aac");
    // save file
    FILE *input_audio_file;
    char input_pcm_file_path[120] = {0};
    sprintf(input_pcm_file_path, "%s%s", root_path, "/input.pcm");
    if((input_audio_file = fopen(input_pcm_file_path,"wb")) == NULL){
        LOGE(TAG, "decodeAudioData2PCM, failed to create output aac file path");
        return;
    }
    fwrite(audio_buffer, 1, pcm_length, input_audio_file);
    fclose(input_audio_file);

    AudioEncoder *audioEncoder = new AudioEncoder(input_pcm_file_path, aac_full_name, AV_CODEC_ID_AAC);
    audioEncoder->encodePCM2AAC();
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