//
// Created by lyman on 2020/7/31.
//
#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include <stdio.h>
#include "include/libavcodec/avcodec.h"
#define LOGD(format, ...)  __android_log_print(ANDROID_LOG_DEBUG, "(>_<)", format, ##__VA_ARGS__)

JNIEXPORT jstring JNICALL Java_com_lyman_ffmpegsample_controller_BasicFFMpegJNI_getConfiguration
        (JNIEnv *env, jobject js) {
    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return env->NewStringUTF(info);
}

#ifdef __cplusplus
}
#endif
