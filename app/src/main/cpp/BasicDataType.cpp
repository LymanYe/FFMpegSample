//
// Created by lyman on 2020/7/31.
//
#include "BasicDataType.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define CLIP_VALUE(x, minval, maxval) ((unsigned char)((x > maxval) ? maxval : ((x < minval) ? minval : x)))

#ifdef __cplusplus
extern "C" {
#endif

// RGB 格式简介可参考：https://cloud.tencent.com/developer/article/1612538
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_splitRGB24
        (JNIEnv *env, jobject js, jbyteArray byteArray, jint width, jint height, jstring path) {
    char *nativePath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    // 生成图片storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/RGB/r.rgb
    // 使用工具yuvplayer以分辨率width、height格式为Y查看提取的图片分量
    char *rPathStr = (char *) malloc(strlen(nativePath) + strlen("/r.rgb"));
    sprintf(rPathStr, "%s%s", nativePath, "/r.rgb");
    LOGD(TAG, "splitRGB24, r path: %s", rPathStr);
    char *gPathStr = (char *) malloc(strlen(nativePath) + strlen("/g.rgb"));
    sprintf(gPathStr, "%s%s", nativePath, "/g.rgb");
    LOGD(TAG, "splitRGB24, g path: %s", gPathStr);
    char *bPathStr = (char *) malloc(strlen(nativePath) + strlen("/b.rgb"));
    sprintf(bPathStr, "%s%s", nativePath, "/b.rgb");
    LOGD(TAG, "splitRGB24, b path: %s", bPathStr);

    FILE *rFile = fopen(rPathStr, "wb");
    if(rFile == NULL) {
        LOGE(TAG, "splitRGB24, rFile create failed path is: %s", rPathStr);
        return;
    }
    FILE *gFile = fopen(gPathStr, "wb");
    if(gFile == NULL) {
        LOGE(TAG, "splitRGB24, gfile create failed path is：%s", gPathStr);
        return;
    }
    FILE *bFile = fopen(bPathStr, "wb");
    if(bFile == NULL) {
        LOGE(TAG, "splitRGB24, bfile create failed path is：%s", bPathStr);
        return;
    }

    jbyte *databyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *pic = (unsigned char *)databyte;

    for (int i = 0; i < width * height * 3; i+=3) {
        fwrite(pic + i, 1, 1, rFile);
        fwrite(pic + i + 1, 1, 1, gFile);
        fwrite(pic + i + 2, 1, 1, bFile);
    }

    free(rPathStr);
    free(gPathStr);
    free(bPathStr);
    fclose(rFile);
    fclose(gFile);
    fclose(bFile);
    env->ReleaseByteArrayElements(byteArray, databyte, 0);
    pic = NULL;
    env->ReleaseStringUTFChars(path, nativePath);
}


// YUV格式简介参考：https://glumes.com/post/ffmpeg/understand-yuv-format/
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_splitYUV420P
        (JNIEnv *env, jobject js, jbyteArray byteArray, jint width, jint height, jstring path) {
    char *nativePath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    // 生成图片storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/YUV/y.yuv
    // 使用工具yuvplayer以分辨率width、height格式为Y查看提取的图片分量
    char *yPathStr = (char *) malloc(strlen(nativePath) + strlen("/y.yuv"));
    sprintf(yPathStr, "%s%s", nativePath, "/y.rgb");
    LOGD(TAG, "splitYUV420P, y path: %s", yPathStr);
    char *uPathStr = (char *) malloc(strlen(nativePath) + strlen("/u.yuv"));
    sprintf(uPathStr, "%s%s", nativePath, "/u.yuv");
    LOGD(TAG, "splitYUV420P, u path: %s", uPathStr);
    char *vPathStr = (char *) malloc(strlen(nativePath) + strlen("/v.yuv"));
    sprintf(vPathStr, "%s%s", nativePath, "/v.yuv");
    LOGD(TAG, "splitYUV420P, v path: %s", vPathStr);

    FILE *yFile = fopen(yPathStr, "wb");
    if(yFile == NULL) {
        LOGE(TAG, "splitYUV420P, yFile create failed path is: %s", yPathStr);
        return;
    }
    FILE *uFile = fopen(uPathStr, "wb");
    if(uFile == NULL) {
        LOGE(TAG, "splitYUV420P, ufile create failed path is：%s", uPathStr);
        return;
    }
    FILE *vFile = fopen(vPathStr, "wb");
    if(vFile == NULL) {
        LOGE(TAG, "splitYUV420P, bfile create failed path is：%s", vPathStr);
        return;
    }

    jbyte *databyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *pic = (unsigned char *)databyte;

    fwrite(pic, 1, width * height, yFile);
    fwrite(pic + width * height, 1, width * height * 1 / 4, uFile);
    fwrite(pic + width * height * 5 / 4, 1, width * height * 1 / 4, vFile);

    free(yPathStr);
    free(uPathStr);
    free(vPathStr);
    fclose(yFile);
    fclose(uFile);
    fclose(vFile);
    env->ReleaseByteArrayElements(byteArray, databyte, 0);
    pic = NULL;
    env->ReleaseStringUTFChars(path, nativePath);
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_convertRGB24ToBMP
        (JNIEnv *env, jobject js, jbyteArray byteArray, jint width, jint height, jstring path) {
    int i = 0, j = 0;
    BmpHead m_BMPHeader = {0};
    InfoHead  m_BMPInfoHeader = {0};
    char bfType[2] = {'B','M'};
    int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
    jbyte *databyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *rgb24_buffer = (unsigned char *)databyte;
    FILE *fp_bmp = NULL;
    char *bmpPath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    if((fp_bmp = fopen(bmpPath,"wb")) == NULL){
        LOGD(TAG, "convertRGB24ToBMP, failed to open bmp path");
        return;
    }

    m_BMPHeader.imageSize = 3 * width * height + header_size;
    m_BMPHeader.startPosition = header_size;

    m_BMPInfoHeader.Length = sizeof(InfoHead);
    m_BMPInfoHeader.width = width;
    //BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
    m_BMPInfoHeader.height = -height;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor= 24;
    m_BMPInfoHeader.realSize = 3 * width * height;

    fwrite(bfType, 1, sizeof(bfType), fp_bmp);
    fwrite(&m_BMPHeader, 1, sizeof(m_BMPHeader), fp_bmp);
    fwrite(&m_BMPInfoHeader, 1, sizeof(m_BMPInfoHeader),fp_bmp);

    //BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
    //It saves pixel data in Little Endian
    //So we change 'R' and 'B'
    for(j = 0; j < height; j++){
        for(i = 0; i < width; i++){
            char temp = rgb24_buffer[(j * width + i) * 3 + 2];
            rgb24_buffer[(j * width + i) * 3 + 2] = rgb24_buffer[(j * width + i) * 3 + 0];
            rgb24_buffer[(j * width + i) *3 + 0] = temp;
        }
    }
    fwrite(rgb24_buffer , 3 * width * height, 1, fp_bmp);

    fclose(fp_bmp);
    env->ReleaseStringUTFChars(path, bmpPath);
    env->ReleaseByteArrayElements(byteArray, databyte, 0);
    rgb24_buffer = NULL;
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_convertRGB24ToYUV420P
        (JNIEnv *env, jobject js, jbyteArray byteArray, jint width, jint height, jstring path) {
    jbyte * rgb_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *rgb_buffer = (unsigned char *)rgb_jbyte;
    unsigned char *outYuv = static_cast<unsigned char *>(malloc(
            width * height * 3 / 2 * sizeof(unsigned char)));

    unsigned char* ptrY, *ptrU, *ptrV, *ptrRGB;
    memset(outYuv, 0, width * height * 3 / 2);
    ptrY = outYuv;
    ptrU = outYuv + width * height;
    ptrV = ptrU + (width * height * 1 / 4);
    unsigned char y, u, v, r, g, b;
    for (int j = 0; j < height; j++){
        ptrRGB = rgb_buffer + width * j * 3 ;
        for (int i = 0;i < width; i++){
            r = *(ptrRGB++);
            g = *(ptrRGB++);
            b = *(ptrRGB++);
            y = (unsigned char)( ( 66 * r + 129 * g +  25 * b + 128) >> 8) + 16  ;
            u = (unsigned char)( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ;
            v = (unsigned char)( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ;
            *(ptrY++) = CLIP_VALUE(y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0)
                *(ptrU++) = CLIP_VALUE(u, 0, 255);
            else if (i % 2 == 0)
                *(ptrV++) = CLIP_VALUE(v, 0, 255);
        }
    }

    FILE *yuvFile = NULL;
    char *yuvPath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    if((yuvFile = fopen(yuvPath,"wb")) == NULL){
        LOGD(TAG, "convertRGB24ToYUV420P, failed to open yuv path");
        return;
    }
    fwrite(outYuv, 1, width * height * 3 / 2, yuvFile);

    env->ReleaseStringUTFChars(path, yuvPath);
    fclose(yuvFile);
    free(outYuv);
    env->ReleaseByteArrayElements(byteArray, rgb_jbyte, 0);
    rgb_buffer = NULL;
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_setYUV420PGray
        (JNIEnv *env, jobject js, jbyteArray byteArray, jint width, jint height, jstring path) {
    jbyte * yuv_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *yuv_buffer = (unsigned char *)yuv_jbyte;

    // set U & V pixel value to 128
    memset(yuv_buffer + width * height, 128, width * height / 2);

    FILE *yuvFile = NULL;
    char *yuvPath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    if((yuvFile = fopen(yuvPath,"wb")) == NULL){
        LOGD(TAG, "setYUV420PGray, failed to open yuv path");
        return;
    }
    fwrite(yuv_buffer, 1, width * height * 3 / 2, yuvFile);

    env->ReleaseStringUTFChars(path, yuvPath);
    fclose(yuvFile);
    env->ReleaseByteArrayElements(byteArray, yuv_jbyte, 0);
}

#ifdef __cplusplus
}
#endif
