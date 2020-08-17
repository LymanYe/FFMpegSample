//
// Created by Lyman on 2020/8/17.
//

#ifndef FFMPEGSAMPLE_BASE_H
#define FFMPEGSAMPLE_BASE_H
#include <jni.h>
#include <android/log.h>

// Print log
#define LOGD(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)
#define LOGE(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
#define LOGV(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#define LOGW(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)

#endif //FFMPEGSAMPLE_BASE_H
