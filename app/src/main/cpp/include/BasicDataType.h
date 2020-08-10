//
// Created by lyman on 2020/8/4.
//

#ifndef FFMPEGSAMPLE_BASICDATATYPE_H
#define FFMPEGSAMPLE_BASICDATATYPE_H

#include <jni.h>
#include <android/log.h>


// Print log
#define LOGD(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)
#define LOGE(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
#define LOGV(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
#define LOGW(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)
#define TAG "BasicDataType"


// BMP Head完整定义
//typedef  struct  tagBITMAPFILEHEADER
//{
//    unsigned short int bfType;       //位图文件的类型，必须为BM
//    unsigned long bfSize;       //文件大小，以字节为单位
//    unsigned short int bfReserverd1; //位图文件保留字，必须为0
//    unsigned short int bfReserverd2; //位图文件保留字，必须为0
//    unsigned long bfbfOffBits;  //位图文件头到数据的偏移量，以字节为单位
//}
typedef struct
{
    long imageSize;
    long blank;
    long startPosition;
}BmpHead;

//Bitmap info header完整定义
//typedef  struct  tagBITMAPINFOHEADER
//{
//    long biSize;                        //该结构大小，字节为单位
//    long  biWidth;                     //图形宽度以象素为单位
//    long  biHeight;                     //图形高度以象素为单位
//    short int  biPlanes;               //目标设备的级别，必须为1
//    short int  biBitcount;             //颜色深度，每个象素所需要的位数
//    short int  biCompression;        //位图的压缩类型
//    long  biSizeImage;              //位图的大小，以字节为单位
//    long  biXPelsPermeter;       //位图水平分辨率，每米像素数
//    long  biYPelsPermeter;       //位图垂直分辨率，每米像素数
//    long  biClrUsed;            //位图实际使用的颜色表中的颜色数
//    long  biClrImportant;       //位图显示过程中重要的颜色数
//}BITMAPINFOHEADER;

typedef struct
{
    long  Length;
    long  width;
    long  height;
    unsigned short  colorPlane;
    unsigned short  bitColor;
    long  zipFormat;
    long  realSize;
    long  xPels;
    long  yPels;
    long  colorUse;
    long  colorImportant;
}InfoHead;

//// WAVE_HEADER字段含义
//typedef struct{
//    char          ChunkID[4];//内容为"RIFF"
//    unsigned long ChunkSize;//存储文件的字节数（不包含ChunkID和ChunkSize这8个字节）
//    char          Format[4];//内容为"WAVE"
//}WAVE_HEADER;
typedef struct WAVE_HEADER{
    char         fccID[4];
    unsigned   long    dwSize;
    char         fccType[4];
}WAVE_HEADER;
//// WAVE_FMT字段含义
//typedef struct{
//    char          Subchunk1ID[4];//内容为"fmt "
//    unsigned long  Subchunk1Size;//存储该子块的字节数（不含前面的Subchunk1ID和Subchunk1Size这8个字节）
//    unsigned short AudioFormat;//存储音频文件的编码格式，例如若为PCM则其存储值为1，若为其他非PCM格式的则有一定的压缩。
//    unsigned short NumChannels;//通道数，单通道(Mono)值为1，双通道(Stereo)值为2，等等
//    unsigned long  SampleRate;//采样率，如8k，44.1k等
//    unsigned long  ByteRate;//每秒存储的bit数，其值=SampleRate * NumChannels * BitsPerSample/8
//    unsigned short BlockAlign;//块对齐大小，其值=NumChannels * BitsPerSample/8
//    unsigned short BitsPerSample;//每个采样点的bit数，一般为8,16,32等。
//}WAVE_FMT;
typedef struct WAVE_FMT{
    char         fccID[4];
    unsigned   long       dwSize;
    unsigned   short     wFormatTag;
    unsigned   short     wChannels;
    unsigned   long       dwSamplesPerSec;
    unsigned   long       dwAvgBytesPerSec;
    unsigned   short     wBlockAlign;
    unsigned   short     uiBitsPerSample;
}WAVE_FMT;
//// WAVE_DATA字段含义
//typedef struct{
//    char          Subchunk2ID[4];//内容为“data”
//    unsigned long Subchunk2Size;//内容为接下来的正式的数据部分的字节数，其值=NumSamples * NumChannels * BitsPerSample/8
//}WAVE_DATA;
typedef struct WAVE_DATA{
    char       fccID[4];
    unsigned long dwSize;
}WAVE_DATA;
#endif //FFMPEGSAMPLE_BASICDATATYPE_H
