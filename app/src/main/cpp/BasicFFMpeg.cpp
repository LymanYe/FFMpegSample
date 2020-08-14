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

#ifdef __cplusplus
}
#endif
