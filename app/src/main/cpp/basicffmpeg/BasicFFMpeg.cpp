//
// Created by lyman on 2020/7/31.
//
#include "BasicFFMpeg.h"
#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include "AudioEncoder.h"
#include "AudioFilter.h"
#include "VideoFilter.h"
#include "VideoEncoder.h"
#include "SwScale.h"
#include "Resample.h"
#include "DeMuxing.h"
#include "Muxing.h"
#include "EncodeMuxing.h"

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
        LOGE(TAG, "encodePCMData2AAC, failed to create output aac file path");
        return;
    }
    fwrite(audio_buffer, 1, pcm_length, input_audio_file);
    fclose(input_audio_file);

    LOGD(TAG, "encodePCMData2AAC");
    AudioEncoder *audioEncoder = new AudioEncoder(input_pcm_file_path, aac_full_name, AV_CODEC_ID_AAC, 64000, AV_SAMPLE_FMT_FLTP);
    audioEncoder->encodePCM2AAC();
}


// encode pcm s16le to AAC
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encodePCMData2MP3
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring aacDir) {
    jbyte *pcm_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *audio_buffer = (unsigned char *)pcm_array;
    int pcm_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_aac_dir = jstring2cchar(env, aacDir);
    char mp3_full_name[150];
    sprintf(mp3_full_name, "%s%s", save_aac_dir, "/encoded_output.mp3");
    // save file
    FILE *input_audio_file;
    char input_pcm_file_path[120] = {0};
    sprintf(input_pcm_file_path, "%s%s", root_path, "/input.pcm");
    if((input_audio_file = fopen(input_pcm_file_path,"wb")) == NULL){
        LOGE(TAG, "encodePCMData2MP3, failed to create output mp3 file path");
        return;
    }
    fwrite(audio_buffer, 1, pcm_length, input_audio_file);
    fclose(input_audio_file);

    LOGD(TAG, "encodePCMData2MP3");
    AudioEncoder *audioEncoder = new AudioEncoder(input_pcm_file_path, mp3_full_name, AV_CODEC_ID_MP3, 64000, AV_SAMPLE_FMT_FLTP);
    audioEncoder->encodePCM2MP3();
}


// encode pcm s16le single tone to MP2
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encodeS16leSingleToneData2MP2
        (JNIEnv *env, jobject js, jstring rootDir) {
    LOGE(TAG, "encodeS16leSingleToneData2MP2");
    char *save_mp2_dir = jstring2cchar(env, rootDir);
    char mp2_full_name[150];
    sprintf(mp2_full_name, "%s%s", save_mp2_dir, "/encoded_output.mp2");


    AudioEncoder *audioEncoder = new AudioEncoder(NULL, mp2_full_name, AV_CODEC_ID_MP2, 64000, AV_SAMPLE_FMT_S16);
    audioEncoder->encodeSingleToneSound();
}


// encode yuv420p to h264
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encodeYUV420PData2H264
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath, jstring yuvDir, jint width, jint height) {
    jbyte *yuv_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *yuv_buffer = (unsigned char *)yuv_array;
    int yuv_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *save_h264_dir = jstring2cchar(env, yuvDir);
    char h264_full_name[150];
    sprintf(h264_full_name, "%s%s", save_h264_dir, "/output.h264");
    // save file
    FILE *input_yuv_file;
    char input_yuv_file_path[120] = {0};
    sprintf(input_yuv_file_path, "%s%s", root_path, "/input.yuv");
    if((input_yuv_file = fopen(input_yuv_file_path,"wb")) == NULL){
        LOGE(TAG, "encodeYUV420PData2H264, failed to create input yuv file path");
        return;
    }
    int ret = fwrite(yuv_buffer, 1, yuv_length, input_yuv_file);
    LOGD(TAG_FFMPEG, "encodeYUV420PData2H264, fwrite size = %d, path = %s", ret, input_yuv_file_path);
    fclose(input_yuv_file);

    VideoEncoder *videoEncoder = new VideoEncoder(input_yuv_file_path, h264_full_name, width, height, AV_CODEC_ID_H264);
    videoEncoder->encodeYUV420PImage2H264();
}


// encode yuv420p to h264
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encodeYUV420PSingleData2H264
        (JNIEnv *env, jobject js, jstring rootOutputPath) {
    char *root_path = jstring2cchar(env, rootOutputPath);
    char h264_full_path[150];
    sprintf(h264_full_path, "%s%s", root_path, "/output_single_data.h264");

    VideoEncoder *videoEncoder = new VideoEncoder(h264_full_path, AV_CODEC_ID_H264);
    videoEncoder->encodeGenerateYUV420P2H264();

}



JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_basicFilterPCMData
        (JNIEnv *env, jobject js, jint duration) {
    AudioFilter *audioFilter = new AudioFilter();
    audioFilter->initSinWavePCMData(20);
    audioFilter->filterSinWavePCMData();
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_decodeAudioAndFillFilter
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath) {
    jbyte *mp4_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *mp4_buffer = (unsigned char *)mp4_array;
    int mp4_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char mp4_full_name[150];
    sprintf(mp4_full_name, "%s%s", root_path, "/output.mp4");
    // save file
    FILE *input_mp4_file;
    if((input_mp4_file = fopen(mp4_full_name,"wb")) == NULL){
        LOGE(TAG, "decodeAudioAndFillFilter, failed to create output mp4 file path");
        return;
    }
    fwrite(mp4_buffer, 1, mp4_length, input_mp4_file);
    fclose(input_mp4_file);

    AudioFilter *audioFilter = new AudioFilter();
    audioFilter->decodeAudioAndInitFilter(mp4_full_name);
    audioFilter->sendPCM2Filter();
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_yuv420PAndFilter
        (JNIEnv *env, jobject js, jbyteArray byteArray, jint width, jint height, jstring rootOutputPath) {
    jbyte *yuv_array = jbyteArray2cbyte(env, byteArray);
    unsigned char *yuv_buffer = (unsigned char *)yuv_array;
    int yuv_length = env->GetArrayLength(byteArray);
    char *root_path = jstring2cchar(env, rootOutputPath);
    char input_yuv_path[150];
    sprintf(input_yuv_path, "%s%s", root_path, "/input.yuv");
    char output_yuv_path[150];
    sprintf(output_yuv_path, "%s%s", root_path, "/output.yuv");
    // save file
    FILE *input_yuv_file;
    if((input_yuv_file = fopen(input_yuv_path,"wb")) == NULL){
        LOGE(TAG, "yuv420PAndFilter, failed to create output yuv file path");
        return;
    }
    fwrite(yuv_buffer, 1, yuv_length, input_yuv_file);
    fclose(input_yuv_file);

    VideoFilter *videoFilter = new VideoFilter();
    videoFilter->initSingleYUVFile(input_yuv_path, output_yuv_path, width, height);
    videoFilter->initFilter();
    videoFilter->filterFrame();

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


// use libswscale convert yuv420p to rgb24
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_swscaleGenerateYUV420P2RGB24
        (JNIEnv *env, jobject js, jstring rootOutputPath) {
    char *root_path = jstring2cchar(env, rootOutputPath);

    SwScale *swScale = new SwScale(root_path);
    swScale->ConvertYUV420P2RGB();
}


// use libswresample convert dbl to s16 pcm
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_resampleDBL2S16PCM
        (JNIEnv *env, jobject js, jstring rootOutputPath) {
    char *root_path = jstring2cchar(env, rootOutputPath);

    LOGD(TAG_FFMPEG, "resampleDBL2S16PCM, output file path = %s", root_path);

    Resample * resample = new Resample(root_path);
    resample->doResample();
}


// h264+mp3 format mp4 demuxing, reference: https://blog.csdn.net/leixiaohua1020/article/details/39767055
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encapsulationFormatSampleDeMuxer
        (JNIEnv *env, jobject js, jstring inputPath, jstring rootOutputPath) {
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *input_path = jstring2cchar(env, inputPath);

    DeMuxing *deMuxing = new DeMuxing();
    deMuxing->initDemuxingSample(input_path, root_path);
    deMuxing->demuxingSample();
}


// h264+aac format mp4 demuxing, reference: https://blog.csdn.net/leixiaohua1020/article/details/39802819
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encapsulationFormatDeMuxer
        (JNIEnv *env, jobject js, jstring inputPath, jstring rootOutputPath) {
    char *root_path = jstring2cchar(env, rootOutputPath);
    char *input_path = jstring2cchar(env, inputPath);

    DeMuxing *deMuxing = new DeMuxing();
    deMuxing->initDemuxing(input_path, root_path);
    deMuxing->demuxing();
}


// h264 + acc to mp4, reference: https://blog.csdn.net/leixiaohua1020/article/details/39802913
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encapsulationFormatMuxer
        (JNIEnv *env, jobject js, jstring inputPathVideo, jstring inputPathAudio, jstring outputPath) {
    char *video_path = jstring2cchar(env, inputPathVideo);
    char *audio_path = jstring2cchar(env, inputPathAudio);
    char *output_path = jstring2cchar(env, outputPath);

    LOGD(TAG_FFMPEG, "encapsulationFormatMuxer, "
                     "input video path = %s; \n"
                     "input audio path = %s; \n"
                     "output path = %s", video_path, audio_path, output_path);

    Muxing *muxing = new Muxing();
    muxing->initMuxing(video_path, audio_path, output_path);
    muxing->doMuxing();
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encapsulationFormatReMuxer
        (JNIEnv *env, jobject js, jstring inputPathVideo, jstring outputPath) {
    char *video_path = jstring2cchar(env, inputPathVideo);
    char *output_path = jstring2cchar(env, outputPath);

    Muxing *muxing = new Muxing();
    muxing->initReMuxing(video_path, output_path);
    muxing->doReMuxing();
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_encapsulationFormatEncodeAndMuxer
        (JNIEnv *env, jobject js, jstring outputPath) {
    char *output_path = jstring2cchar(env, outputPath);

    EncodeMuxing *encodeMuxing = new EncodeMuxing();
    encodeMuxing->init(output_path);
    encodeMuxing->encodeAndMuxer();
}


#ifdef __cplusplus
}
#endif
