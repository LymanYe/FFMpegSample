//
// Created by danale on 2020/8/13.
//

#ifndef FFMPEGSAMPLE_BASICFFMPEG_H
#define FFMPEGSAMPLE_BASICFFMPEG_H
#include <jni.h>
#include <android/log.h>


// Print log
#define LOGD(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)
#define LOGE(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
#define LOGV(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#define LOGW(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)
#define TAG "BasicFFMpeg"
#define TAG_FFMPEG "FFMpeg"
#endif //FFMPEGSAMPLE_BASICFFMPEG_H
