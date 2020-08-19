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


// PCM格式简介参考：https://www.cnblogs.com/CoderTian/p/6657844.html
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_splitPCM16LE
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring path) {
    char *nativePath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    // 生成图片storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/PCM/left.pcm
    // 使用工具Audacity查看提取出来的左右声道音频数据
    char *leftPCMPathStr = (char *) malloc(strlen(nativePath) + strlen("/left.pcm"));
    sprintf(leftPCMPathStr, "%s%s", nativePath, "/left.pcm");
    LOGD(TAG, "splitPCM16LE, left pcm path: %s", leftPCMPathStr);
    char *rightPCMPathStr = (char *) malloc(strlen(nativePath) + strlen("/right.pcm"));
    sprintf(rightPCMPathStr, "%s%s", nativePath, "/right.pcm");
    LOGD(TAG, "splitPCM16LE, right pcm path: %s", rightPCMPathStr);

    FILE *leftPCMFile = fopen(leftPCMPathStr, "wb");
    if(leftPCMFile == NULL) {
        LOGE(TAG, "splitPCM16LE, left pcm file create failed path is: %s", leftPCMPathStr);
        return;
    }
    FILE *rightPCMFile = fopen(rightPCMPathStr, "wb");
    if(rightPCMFile == NULL) {
        LOGE(TAG, "splitPCM16LE, right pcm file create failed path is：%s", rightPCMPathStr);
        return;
    }

    jbyte *databyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *pcm = (unsigned char *)databyte;
    int dataLength = env->GetArrayLength(byteArray);

    // 因为PCM音频数据是按照LRLRLR的方式来存储左右声道的音频数据的，所以我们可以通过将它们交叉的读出来的方式来分离左右声道的数据
    for(int index = 0; index < dataLength; index += 4) {
        fwrite(pcm + index, 1, 2, leftPCMFile);
        fwrite(pcm + index + 2, 1, 2, rightPCMFile);
    }

    free(leftPCMPathStr);
    free(rightPCMPathStr);
    fclose(leftPCMFile);
    fclose(rightPCMFile);
    env->ReleaseByteArrayElements(byteArray, databyte, 0);
    pcm = NULL;
    env->ReleaseStringUTFChars(path, nativePath);
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_halfPCMLeftVolume
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring path) {
    char *nativePath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    // 生成图片storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/PCM/half_left.pcm
    // 使用工具Audacity查看提取出来的左右声道音频数据
    char *leftPCMPathStr = (char *) malloc(strlen(nativePath) + strlen("/half_left.pcm"));
    sprintf(leftPCMPathStr, "%s%s", nativePath, "/half_left.pcm");
    LOGD(TAG, "splitPCM16LE, left pcm path: %s", leftPCMPathStr);
    char *rightPCMPathStr = (char *) malloc(strlen(nativePath) + strlen("/right.pcm"));
    sprintf(rightPCMPathStr, "%s%s", nativePath, "/right.pcm");
    LOGD(TAG, "splitPCM16LE, right pcm path: %s", rightPCMPathStr);

    FILE *leftPCMFile = fopen(leftPCMPathStr, "wb");
    if(leftPCMFile == NULL) {
        LOGE(TAG, "splitPCM16LE, left pcm file create failed path is: %s", leftPCMPathStr);
        return;
    }
    FILE *rightPCMFile = fopen(rightPCMPathStr, "wb");
    if(rightPCMFile == NULL) {
        LOGE(TAG, "splitPCM16LE, right pcm file create failed path is：%s", rightPCMPathStr);
        return;
    }

    jbyte *databyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *pcm = (unsigned char *)databyte;
    int dataLength = env->GetArrayLength(byteArray);

    // 因为对于PCM音频数据而言，它的幅值（即该采样点采样值的大小）代表音量的大小，所以我们可以通过减小某个声道的数据的值来实现降低某个声道的音量
    for(int index = 0; index < dataLength; index += 4) {
        short *leftData = reinterpret_cast<short *>(pcm + index);
        *leftData = *leftData / 2;
        fwrite(pcm + index, 1, 2, leftPCMFile);
        fwrite(pcm + index + 2, 1, 2, rightPCMFile);
    }

    free(leftPCMPathStr);
    free(rightPCMPathStr);
    fclose(leftPCMFile);
    fclose(rightPCMFile);
    env->ReleaseByteArrayElements(byteArray, databyte, 0);
    pcm = NULL;
    env->ReleaseStringUTFChars(path, nativePath);
}


// WAV格式简介：http://soundfile.sapp.org/doc/WaveFormat/
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_convertPCM16LEToWAVE
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring path) {
    int channels = 2, sample_rate = 44100, bits = 16;
    WAVE_HEADER   pcmHEADER;
    WAVE_FMT   pcmFMT;
    WAVE_DATA   pcmDATA;

    char *nativeSavePath = const_cast<char *>(env->GetStringUTFChars(path, JNI_FALSE));
    jbyte *databyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *pcm = (unsigned char *)databyte;
    int dataLength = env->GetArrayLength(byteArray);
    FILE *nativeSaveFile = fopen(nativeSavePath, "wb");
    if(nativeSaveFile == NULL) {
        LOGE(TAG, "convertPCM16LEToWAVE, save wav file create failed path is: %s", nativeSavePath);
        return;
    }

    //WAVE_HEADER
    memcpy(pcmHEADER.fccID,"RIFF",strlen("RIFF"));
    memcpy(pcmHEADER.fccType,"WAVE",strlen("WAVE"));
    fseek(nativeSaveFile, sizeof(WAVE_HEADER), SEEK_CUR);
    //WAVE_FMT
    memcpy(pcmFMT.fccID,"fmt ",strlen("fmt "));
    pcmFMT.dwSamplesPerSec = sample_rate;
    pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec * sizeof(pcm) * 2;
    pcmFMT.uiBitsPerSample = bits;
    pcmFMT.dwSize = 16;
    pcmFMT.wBlockAlign = 2;
    pcmFMT.wChannels = channels;
    pcmFMT.wFormatTag = 1;

    fwrite(&pcmFMT, sizeof(WAVE_FMT), 1, nativeSaveFile);
    //WAVE_DATA;
    memcpy(pcmDATA.fccID, "data", strlen("data"));
    pcmDATA.dwSize = 0;
    fseek(nativeSaveFile, sizeof(WAVE_DATA), SEEK_CUR);
    for(int index = 0; index < dataLength; index += 2) {
        pcmDATA.dwSize += 2;
        fwrite(pcm + index, sizeof(unsigned short), 1, nativeSaveFile);
    }

    pcmHEADER.dwSize = 44 + pcmDATA.dwSize;
    rewind(nativeSaveFile);
    fwrite(&pcmHEADER, sizeof(WAVE_HEADER), 1, nativeSaveFile);
    fseek(nativeSaveFile,sizeof(WAVE_FMT),SEEK_CUR);
    fwrite(&pcmDATA, sizeof(WAVE_DATA), 1, nativeSaveFile);

    fclose(nativeSaveFile);
    env->ReleaseByteArrayElements(byteArray, databyte, 0);
    pcm = NULL;
    env->ReleaseStringUTFChars(path, nativeSavePath);
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


// H264格式简介参考：https://zhuanlan.zhihu.com/p/71928833
int FindStartCode2 (unsigned char *Buf){
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; //0x000001?
    else return 1;
}

int FindStartCode3 (unsigned char *Buf){
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;//0x00000001?
    else return 1;
}

FILE *h264bitstream;
int GetAnnexbNALU (NALU_t *nalu){
    int pos = 0;
    int StartCodeFound, rewind;
    unsigned char *Buf;

    if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
        LOGD(TAG, "GetAnnexbNALU, Could not allocate Buf memory\n");

    nalu->startcodeprefix_len=3;

    if(3 != fread(Buf, 1, 3, h264bitstream)){
        free(Buf);
        return 0;
    }
    int info2 = 0, info3 = 0;
    info2 = FindStartCode2 (Buf);
    if(info2 != 1) {
        if(1 != fread(Buf+3, 1, 1, h264bitstream)){
            free(Buf);
            return 0;
        }
        info3 = FindStartCode3(Buf);
        if (info3 != 1){
            free(Buf);
            return -1;
        }
        else {
            pos = 4;
            nalu->startcodeprefix_len = 4;
        }
    }
    else{
        nalu->startcodeprefix_len = 3;
        pos = 3;
    }
    StartCodeFound = 0;
    info2 = 0;
    info3 = 0;

    while (!StartCodeFound){
        if (feof(h264bitstream)){
            nalu->len = (pos - 1)-nalu->startcodeprefix_len;
            memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
            nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
            nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
            nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
            free(Buf);
            return pos - 1;
        }
        Buf[pos++] = fgetc(h264bitstream);
        info3 = FindStartCode3(&Buf[pos - 4]);
        if(info3 != 1)
            info2 = FindStartCode2(&Buf[pos - 3]);
        StartCodeFound = (info2 == 1 || info3 == 1);
    }

    // Here, we have found another start code (and read length of startcode bytes more than we should
    // have.  Hence, go back in the file
    rewind = (info3 == 1)? -4 : -3;

    if (0 != fseek(h264bitstream, rewind, SEEK_CUR)){
        free(Buf);
        LOGE(TAG, "GetAnnexbNALU, Cannot fseek in the bit stream file");
    }

    // Here the Start code, the complete NALU, and the next start code is in the Buf.
    // The size of Buf is pos, pos+rewind are the number of bytes excluding the next
    // start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code
    nalu->len = (pos + rewind) - nalu->startcodeprefix_len;
    memcpy(nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//
    nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
    nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
    nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
    free(Buf);

    return (pos + rewind);
}


JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_analysisH264Format
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath) {
    jbyte *h264_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *h264_buffer = (unsigned char *)h264_jbyte;
    int byteLength = env->GetArrayLength(byteArray);
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(rootOutputPath, JNI_FALSE));

    // save file
    FILE *outputH264File;
    char outputH264FilePath[120] = {0};
    sprintf(outputH264FilePath, "%s%s", outputPath, "/output.h264");
    if((outputH264File = fopen(outputH264FilePath,"wb")) == NULL){
        LOGE(TAG, "analysisH264Format, failed to create output h264 file path");
        return;
    }
    fwrite(h264_buffer, 1, byteLength, outputH264File);
    // save analysis result
    FILE *outputH264AnalysisFile;
    char outputH264AnalysisFilePath[120] = {0};
    sprintf(outputH264AnalysisFilePath, "%s%s", outputPath, "/output.txt");
    if((outputH264AnalysisFile = fopen(outputH264AnalysisFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisH264Format, failed to create output analysis file path");
        return;
    }
    LOGD(TAG, "analysisH264Format, output analysis file path: %s", outputH264AnalysisFilePath);
    LOGD(TAG, "analysisH264Format, output h264 file path: %s", outputH264FilePath);
    h264bitstream = fopen(outputH264FilePath, "rb+");

    int buffersize = 100000;
    NALU_t *naluT = (NALU_t*)calloc (1, sizeof (NALU_t));
    if (naluT == NULL){
        LOGE(TAG, "analysisH264Format, alloc NALU error");
        return;
    }

    naluT -> max_size = buffersize;
    naluT -> buf = (char*)calloc (buffersize, sizeof (char));
    if (naluT -> buf == NULL){
        free (naluT);
        LOGE (TAG, "analysisH264Format, alloc NALU error: n->buf");
        return;
    }

    int data_offset = 0;
    int nal_num= 0;
    fprintf(outputH264AnalysisFile, "%s", "-----+--------- NALU Table ------+---------+\n");
    fprintf(outputH264AnalysisFile, "%s", " NUM |    POS  |    IDC |  TYPE  |   LEN   |\n");
    fprintf(outputH264AnalysisFile, "%s", "-----+---------+--------+--------+---------+\n");

    while(!feof(h264bitstream))
    {
        int data_lenth;
        data_lenth = GetAnnexbNALU(naluT);
        char type_str[20] = {0};
        switch(naluT -> nal_unit_type){
            case NALU_TYPE_SLICE:sprintf(type_str, "%s", "SLICE");break;
            case NALU_TYPE_DPA:sprintf(type_str, "%s", "DPA");break;
            case NALU_TYPE_DPB:sprintf(type_str, "%s", "DPB");break;
            case NALU_TYPE_DPC:sprintf(type_str, "%s", "DPC");break;
            case NALU_TYPE_IDR:sprintf(type_str, "%s", "IDR");break;
            case NALU_TYPE_SEI:sprintf(type_str, "%s", "SEI");break;
            case NALU_TYPE_SPS:sprintf(type_str, "%s", "SPS");break;
            case NALU_TYPE_PPS:sprintf(type_str, "%s", "PPS");break;
            case NALU_TYPE_AUD:sprintf(type_str, "%s", "AUD");break;
            case NALU_TYPE_EOSEQ:sprintf(type_str, "%s", "EOSEQ");break;
            case NALU_TYPE_EOSTREAM:sprintf(type_str, "%s", "EOSTREAM");break;
            case NALU_TYPE_FILL:sprintf(type_str, "%s", "FILL");break;
        }
        char idc_str[20] = {0};
        switch(naluT -> nal_reference_idc>>5){
            case NALU_PRIORITY_DISPOSABLE:sprintf(idc_str, "%s", "DISPOS");break;
            case NALU_PRIRITY_LOW:sprintf(idc_str, "%s", "LOW");break;
            case NALU_PRIORITY_HIGH:sprintf(idc_str, "%s", "HIGH");break;
            case NALU_PRIORITY_HIGHEST:sprintf(idc_str, "%s", "HIGHEST");break;
        }
        fprintf(outputH264AnalysisFile, "%5d| %8d| %7s| %7s| %8d|\n", nal_num, data_offset, idc_str, type_str, naluT->len);
        data_offset = data_offset + data_lenth;
        nal_num++;
    }

    //Free
    if (naluT)
        if (naluT -> buf){
            free(naluT -> buf);
            naluT -> buf = NULL;
        }
    free(naluT);
    fclose(h264bitstream);
    fclose(outputH264AnalysisFile);
    fclose(outputH264File);
    env -> ReleaseStringUTFChars(rootOutputPath, outputPath);
    env->ReleaseByteArrayElements(byteArray, h264_jbyte, 0);
}


int GetNaluDataLen(int startPos, int h265BitsSize, unsigned char *h265Bits)
{
    int parsePos = 0;
    parsePos = startPos;

    while (parsePos < h265BitsSize)
    {
        if (FindStartCode2(&h265Bits[parsePos]))
        {
            return parsePos - startPos;
        }
        else if (FindStartCode3(&h265Bits[parsePos]))
        {
            return parsePos - startPos;
        }
        else
        {
            parsePos++;
        }
    }

    return parsePos - startPos; // if file is end
}

void ParseNaluData(const unsigned int naluLen, unsigned char* const nuluData, FILE *file)
{
    static int naluNum = 0;
    unsigned char *data = NULL;
    char typeStr[20] = {0};

    T_H265_NALU_HEADER h265NaluHeader = {0};
    data = nuluData;
    memset(&h265NaluHeader, 0x0, sizeof(T_H265_NALU_HEADER));
    h265NaluHeader.nal_unit_type = ((data[0]>>1) & 0x3f);
    naluNum++;

    switch (h265NaluHeader.nal_unit_type)
    {
        case HEVC_NAL_TRAIL_N:sprintf(typeStr, "%s", "B SLICE");break;
        case HEVC_NAL_TRAIL_R:sprintf(typeStr, "%s", "P SLICE");break;
        case HEVC_NAL_IDR_W_RADL:sprintf(typeStr, "%s", "IDR");break;
        case HEVC_NAL_VPS:sprintf(typeStr, "%s", "VPS");break;
        case HEVC_NAL_SPS:sprintf(typeStr, "%s", "SPS");break;
        case HEVC_NAL_PPS:sprintf(typeStr, "%s", "PPS");break;
        case HEVC_NAL_SEI_PREFIX:sprintf(typeStr, "%s", "SEI");break;
        default:sprintf(typeStr, "NTYPE(%d)", h265NaluHeader.nal_unit_type);break;
    }

    fprintf(file, "%5d| %7s| %8d|\n", naluNum, typeStr, naluLen);
}


// 参考：https://www.cnblogs.com/leaffei/p/10553783.html
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_analysisH265Format
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath) {
    jbyte *h265_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *h265_buffer = (unsigned char *)h265_jbyte;
    int byteLength = env->GetArrayLength(byteArray);
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(rootOutputPath, JNI_FALSE));

    // save file
    FILE *outputH265File;
    char outputH265FilePath[120] = {0};
    sprintf(outputH265FilePath, "%s%s", outputPath, "/output.h265");
    if((outputH265File = fopen(outputH265FilePath,"wb")) == NULL){
        LOGE(TAG, "analysisH265Format, failed to create output h265 file path");
        return;
    }
    fwrite(h265_buffer, 1, byteLength, outputH265File);
    // save analysis result
    FILE *outputH265AnalysisFile;
    char outputH265AnalysisFilePath[120] = {0};
    sprintf(outputH265AnalysisFilePath, "%s%s", outputPath, "/output.txt");
    if((outputH265AnalysisFile = fopen(outputH265AnalysisFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisH265Format, failed to create output analysis file path");
        return;
    }
    LOGD(TAG, "analysisH265Format, output analysis file path: %s", outputH265AnalysisFilePath);
    LOGD(TAG, "analysisH265Format, output h265 file path: %s", outputH265FilePath);

    int fileLen = 0;
    int naluLen = 0;
    int h265BitsPos = 0; /* h265, hevc; h264, avc系列, Advanced Video Coding */

    unsigned char *h265Bits = NULL;
    unsigned char *naluData = NULL;

    FILE *fp = fopen(outputH265FilePath, "rb+");;
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    h265Bits = (unsigned char*)malloc(fileLen);
    if (!h265Bits)
    {
        LOGE(TAG, "analysisH265Format, maybe file is too long, or memery is not enough!\n");
        fclose(fp);
        return;
    }

    memset(h265Bits, 0x0, fileLen);

    if (fread(h265Bits, 1, fileLen, fp) < 0)
    {
        LOGE(TAG, "analysisH265Format, read file data to h265Bits error!\n");
        fclose(fp);
        free(h265Bits);
        h265Bits = NULL;
        return;
    }
    fclose(fp);

    fprintf(outputH265AnalysisFile, "%s", "-----+--- NALU Table --+\n");
    fprintf(outputH265AnalysisFile, "%s", " NUM |  TYPE |   LEN   |\n");
    fprintf(outputH265AnalysisFile, "%s", "-----+-------+---------+\n");

    while (h265BitsPos < (fileLen-4))
    {
        if (FindStartCode2(&h265Bits[h265BitsPos]))
        {
            naluLen = GetNaluDataLen(h265BitsPos+3, fileLen, h265Bits);
            naluData = (unsigned char*)malloc(naluLen);
            if (naluData)
            {
                memset(naluData, 0x0, naluLen);
                memcpy(naluData, h265Bits+h265BitsPos+3, naluLen);
                ParseNaluData(naluLen, naluData, outputH265AnalysisFile);
                free(naluData);
                naluData = NULL;
            }

            h265BitsPos += (naluLen+3);
        }
        else if (FindStartCode3(&h265Bits[h265BitsPos]))
        {
            naluLen = GetNaluDataLen(h265BitsPos+4, fileLen, h265Bits);

            naluData = (unsigned char*)malloc(naluLen);
            if (naluData)
            {
                memset(naluData, 0x0, naluLen);
                memcpy(naluData, h265Bits+h265BitsPos+4, naluLen);
                ParseNaluData(naluLen, naluData, outputH265AnalysisFile);
                free(naluData);
                naluData = NULL;
            }
            h265BitsPos += (naluLen+4);
        }
        else
        {
            h265BitsPos++;
        }
    }

    if (naluData) {
        free(naluData);
        naluData = NULL;
    }
    if (h265Bits) {
        free(h265Bits);
        h265Bits = NULL;
    }
    fclose(fp);
    fclose(outputH265AnalysisFile);
    fclose(outputH265File);
    env -> ReleaseStringUTFChars(rootOutputPath, outputPath);
    env->ReleaseByteArrayElements(byteArray, h265_jbyte, 0);
}


int getADTSframe(unsigned char* buffer, int buf_size, unsigned char* data ,int* data_size){
    int size = 0;
    if(!buffer || !data || !data_size){
        return -1;
    }

    while(1){
        if(buf_size < 7)
            return -1;
        //Sync words
        if((buffer[0] == 0xff) && ((buffer[1] & 0xf0) == 0xf0) ){
            size |= ((buffer[3] & 0x03) <<11);     //high 2 bit
            size |= buffer[4]<<3;                //middle 8 bit
            size |= ((buffer[5] & 0xe0)>>5);        //low 3bit
            break;
        }
        --buf_size;
        ++buffer;
    }

    if(buf_size < size)
        return 1;

    memcpy(data, buffer, size);
    *data_size = size;
    return 0;
}


// AAC格式参考：https://blog.csdn.net/lq496387202/article/details/81014474
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_analysisAACFormat
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath) {
    jbyte *aac_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *aac_buffer = (unsigned char *)aac_jbyte;
    int byteLength = env->GetArrayLength(byteArray);
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(rootOutputPath, JNI_FALSE));

    // save file
    FILE *outputAACFile;
    char outputAACFilePath[120] = {0};
    sprintf(outputAACFilePath, "%s%s", outputPath, "/output.aac");
    if((outputAACFile = fopen(outputAACFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisAACFormat, failed to create output aac file path");
        return;
    }
    fwrite(aac_buffer, 1, byteLength, outputAACFile);
    // save analysis result
    FILE *outputAACAnalysisFile;
    char outputAACAnalysisFilePath[120] = {0};
    sprintf(outputAACAnalysisFilePath, "%s%s", outputPath, "/output.txt");
    if((outputAACAnalysisFile = fopen(outputAACAnalysisFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisAACFormat, failed to create output analysis file path");
        return;
    }
    LOGD(TAG, "analysisAACFormat, output analysis file path: %s", outputAACAnalysisFilePath);
    LOGD(TAG, "analysisAACFormat, output aac file path: %s", outputAACFilePath);

    int data_size = 0;
    int size = 0;
    int cnt = 0;
    int offset = 0;
    unsigned char *aacframe=(unsigned char *)malloc(1024*5);
    unsigned char *aacbuffer=(unsigned char *)malloc(1024*1024);

    FILE *aacFile = fopen(outputAACFilePath, "rb");
    if(!aacFile){
        LOGE(TAG, "analysisAACFormat, Open file error");
        return;
    }

    fprintf(outputAACAnalysisFile,"%s", "-----+- ADTS Frame Table -+------+\n");
    fprintf(outputAACAnalysisFile,"%s", " NUM | Profile | Frequency| Size |\n");
    fprintf(outputAACAnalysisFile,"%s", "-----+---------+----------+------+\n");

    while(!feof(aacFile)){
        data_size = fread(aacbuffer + offset, 1, 1024*1024 - offset, aacFile);
        unsigned char* input_data = aacbuffer;
        while(1)
        {
            int ret = getADTSframe(input_data, data_size, aacframe, &size);
            if(ret == -1)
                break;
            else if(ret == 1){
                memcpy(aacbuffer, input_data, data_size);
                offset = data_size;
                break;
            }
            char profile_str[10] = {0};
            char frequence_str[10] = {0};

            unsigned char profile = aacframe[2] & 0xC0;
            profile = profile >> 6;
            switch(profile){
                case 0: sprintf(profile_str, "%s", "Main");break;
                case 1: sprintf(profile_str, "%s", "LC");break;
                case 2: sprintf(profile_str, "%s", "SSR");break;
                default:sprintf(profile_str, "%s", "unknown");break;
            }

            unsigned char sampling_frequency_index = aacframe[2] & 0x3C;
            sampling_frequency_index = sampling_frequency_index >> 2;
            switch(sampling_frequency_index){
                case 0: sprintf(frequence_str, "%s", "96000Hz");break;
                case 1: sprintf(frequence_str, "%s", "88200Hz");break;
                case 2: sprintf(frequence_str, "%s", "64000Hz");break;
                case 3: sprintf(frequence_str, "%s", "48000Hz");break;
                case 4: sprintf(frequence_str, "%s", "44100Hz");break;
                case 5: sprintf(frequence_str, "%s", "32000Hz");break;
                case 6: sprintf(frequence_str, "%s", "24000Hz");break;
                case 7: sprintf(frequence_str, "%s", "22050Hz");break;
                case 8: sprintf(frequence_str, "%s", "16000Hz");break;
                case 9: sprintf(frequence_str, "%s", "12000Hz");break;
                case 10: sprintf(frequence_str, "%s", "11025Hz");break;
                case 11: sprintf(frequence_str, "%s", "8000Hz");break;
                default:sprintf(frequence_str, "%s", "unknown");break;
            }

            fprintf(outputAACAnalysisFile,"%5d| %8s|  %8s| %5d|\n", cnt, profile_str, frequence_str, size);
            data_size -= size;
            input_data += size;
            cnt++;
        }
    }

    fclose(outputAACAnalysisFile);
    fclose(outputAACFile);
    env -> ReleaseStringUTFChars(rootOutputPath, outputPath);
    env->ReleaseByteArrayElements(byteArray, aac_jbyte, 0);
}


//reverse_bytes - turn a BigEndian byte array into a LittleEndian integer
uint reverse_bytes(byte *p, char c) {
    int r = 0;
    int i;
    for (i=0; i<c; i++)
        r |= ( *(p+i) << (((c-1)*8)-8*i));
    return r;
}


int getw(FILE *fp)
{
    char s[2];
    s[0] = getc(fp);
    s[1] = getc(fp);
    return *(int*)s;
}

//如果字节序为big-endian，返回true;
//反之为   little-endian，返回false
int IsBig_Endian()
{
    unsigned short test = 0x1122;
    if(*( (unsigned char*) &test ) == 0x11)
        return JNI_TRUE;
    else
        return JNI_FALSE;
}

// FLV格式参考：https://blog.csdn.net/leixiaohua1020/article/details/17934487
// 提取数据参考：https://www.w3xue.com/exp/article/20193/23148.html
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_analysisFLVFormat
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath) {
    jbyte *flv_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *flv_buffer = (unsigned char *)flv_jbyte;
    int byteLength = env->GetArrayLength(byteArray);
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(rootOutputPath, JNI_FALSE));
    LOGD(TAG, "analysisFLVFormat, is big endian start: %s", "lyman");
    LOGD(TAG, "analysisFLVFormat, is big endian: %d", IsBig_Endian());
    // save file
    FILE *outputFLVFile;
    char outputFLVFilePath[120] = {0};
    sprintf(outputFLVFilePath, "%s%s", outputPath, "/output.flv");
    if((outputFLVFile = fopen(outputFLVFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisFLVFormat, failed to create output flv file path");
        return;
    }
    fwrite(flv_buffer, 1, byteLength, outputFLVFile);
    // save analysis result
    FILE *outputFLVAnalysisFile;
    char outputFLVAnalysisFilePath[120] = {0};
    sprintf(outputFLVAnalysisFilePath, "%s%s", outputPath, "/output.txt");
    if((outputFLVAnalysisFile = fopen(outputFLVAnalysisFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisFVLFormat, failed to create output analysis file path");
        return;
    }
    LOGD(TAG, "analysisFLVFormat, output analysis file path: %s", outputFLVAnalysisFilePath);
    LOGD(TAG, "analysisFLVFormat, output flv file path: %s", outputFLVFilePath);

    int dataLen = 0;

    FILE *fp = NULL;
    FILE *extractAudioFile = NULL;
    FILE *extractVideoFile = NULL;

    unsigned char *tagData = NULL;

    unsigned char flvHeaderData[MIN_FLV_HEADER_LEN+1] = {0};
    unsigned char preTagSizeData[MAX_PRE_TAG_SIZE_LEN+1] = {0};
    unsigned char tagHeaderData[MAX_TAG_HEADER_LEN+1] = {0};

    T_FLV_TAG_HEADER tagHeader = {0};

    fp = fopen(outputFLVFilePath, "rb");
    if (!fp)
    {
        LOGD(TAG, "analysisFLVFormat, open file [%s] error!\n", outputFLVFilePath);
        return;
    }

    memset(flvHeaderData, 0x0, sizeof(flvHeaderData));

    dataLen = fread(flvHeaderData, 1, MIN_FLV_HEADER_LEN, fp);
    if (dataLen != MIN_FLV_HEADER_LEN)
    {
        LOGD(TAG, "analysisFLVFormat, read flv header error!\n");
        return;
    }

    flvHeaderData[MIN_FLV_HEADER_LEN] = '\0';

    DealFlvHeader(flvHeaderData, outputFLVAnalysisFile);

    fprintf(outputFLVAnalysisFile,"%s", "-----+- FLV TAG Table -+------+\n");
    fprintf(outputFLVAnalysisFile,"%s", " NUM | Type | Size | TimeStamp \n");
    fprintf(outputFLVAnalysisFile,"%s", "-----+---------+----------+--------+\n");
    while (1)
    {
        memset(preTagSizeData, 0x0, sizeof(preTagSizeData));

        dataLen = fread(preTagSizeData, 1, MAX_PRE_TAG_SIZE_LEN, fp);
        if (dataLen != MAX_PRE_TAG_SIZE_LEN)
        {
            break;
        }

        preTagSizeData[MAX_PRE_TAG_SIZE_LEN] = '\0';

        fprintf(outputFLVAnalysisFile, "previousTagSize: %d\n", (preTagSizeData[0]<<24) | (preTagSizeData[1]<<16) | (preTagSizeData[2]<<8) | preTagSizeData[3]);

        memset(tagHeaderData, 0x0, sizeof(tagHeaderData));

        dataLen = fread(tagHeaderData, 1, MAX_TAG_HEADER_LEN, fp);
        if (dataLen != MAX_TAG_HEADER_LEN)
        {
            continue;
        }

        memset(&tagHeader, 0x0, sizeof(T_FLV_TAG_HEADER));

        DealTagHeader(tagHeaderData, &tagHeader, outputFLVAnalysisFile);

        tagData = (unsigned char*)malloc(tagHeader.data_size);
        if (!tagData)
        {
            continue;
        }

        memset(tagData, 0x0, tagHeader.data_size);

        dataLen = fread(tagData, 1, tagHeader.data_size, fp);
        if (dataLen != tagHeader.data_size)
        {
            continue;
        }

        DealTagData(tagData, tagHeader.type, tagHeader.data_size);

        // todo save extract h264 & aac file
        switch (tagHeader.type){
            // video
            case 0x9:
                break;

            // audio
            case 0x8:
                if(extractAudioFile == NULL){
                    char outputFLVAudioPath[120] = {0};
                    sprintf(outputFLVAudioPath, "%s%s", outputPath, "/extract_output.aac");
                    extractAudioFile = fopen(outputFLVAudioPath, "wb");
                }
                break;
        }
        free(tagData);
        tagData = NULL;
    }

    if(extractAudioFile != NULL)
        fclose(extractAudioFile);
    fclose(fp);
    fclose(outputFLVAnalysisFile);
    fclose(outputFLVFile);
    env -> ReleaseStringUTFChars(rootOutputPath, outputPath);
    env->ReleaseByteArrayElements(byteArray, flv_jbyte, 0);
}


// MP4格式参考:https://www.cnblogs.com/ranson7zop/p/7889272.html http://www.52rd.com/Blog/wqyuwss/559/
// MP4格式解析参考：https://www.cnblogs.com/Kingfans/p/7170478.html
JNIEXPORT void JNICALL Java_com_lyman_ffmpegsample_controller_BasicDataTypeJNI_analysisMP4Format
        (JNIEnv *env, jobject js, jbyteArray byteArray, jstring rootOutputPath) {
    jbyte *mp4_jbyte = env->GetByteArrayElements(byteArray, 0);
    unsigned char *mp4_buffer = (unsigned char *)mp4_jbyte;
    int byteLength = env->GetArrayLength(byteArray);
    char *outputPath = const_cast<char *>(env->GetStringUTFChars(rootOutputPath, JNI_FALSE));
    // save file
    FILE *outputMP4File;
    char outputMP4FilePath[120] = {0};
    sprintf(outputMP4FilePath, "%s%s", outputPath, "/output.mp4");
    if((outputMP4File = fopen(outputMP4FilePath,"wb")) == NULL){
        LOGE(TAG, "analysisMP4Format, failed to create output mp4 file path");
        return;
    }
    fwrite(mp4_buffer, 1, byteLength, outputMP4File);
    // save analysis result
    FILE *outputMP4AnalysisFile;
    char outputMP4AnalysisFilePath[120] = {0};
    sprintf(outputMP4AnalysisFilePath, "%s%s", outputPath, "/output.txt");
    if((outputMP4AnalysisFile = fopen(outputMP4AnalysisFilePath,"wb")) == NULL){
        LOGE(TAG, "analysisMP4Format, failed to create output analysis file path");
        return;
    }
    LOGD(TAG, "analysisMP4Format, output analysis file path: %s", outputMP4AnalysisFilePath);
    LOGD(TAG, "analysisMP4Format, output mp4 file path: %s", outputMP4FilePath);

    FILE *fp = NULL;
    fp = fopen(outputMP4FilePath, "rb");
    if (!fp)
    {
        LOGD(TAG, "analysisMP4Format, open file [%s] error!\n", outputMP4FilePath);
        return;
    }
    unsigned char boxSize[MAX_BOX_SIZE_LEN] = {0};
    T_BOX box = {0};

    while (1)
    {
        memset(&box, 0x0, sizeof(T_BOX));

        if (fread(boxSize, 1, 4, fp) <= 0)
        {
            break;
        }

        box.boxHeader.boxSize = boxSize[0] << 24 | boxSize[1] << 16 | boxSize[2] << 8 | boxSize[3];

        fread(box.boxHeader.boxType, 1, 4, fp);

        box.boxHeader.boxType[MAX_BOX_TYPE_LEN] = '\0';

        box.boxData = (unsigned char*)malloc(box.boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);
        if (!box.boxData)
        {
            LOGE(TAG, "analysisMP4Format, malloc data error!\n");
            break;
        }

        fread(box.boxData, 1, box.boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN, fp);

        /* deal box data */
        DealBox(&box);

        /* free box */
        free(box.boxData);

        box.boxData = NULL;
    }

    fclose(fp);
    fclose(outputMP4AnalysisFile);
    fclose(outputMP4File);
    env -> ReleaseStringUTFChars(rootOutputPath, outputPath);
    env->ReleaseByteArrayElements(byteArray, mp4_jbyte, 0);
}
#ifdef __cplusplus
}
#endif
