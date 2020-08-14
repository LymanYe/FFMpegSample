//
// Created by lyman on 2020/8/4.
//

#ifndef FFMPEGSAMPLE_BASICDATATYPE_H
#define FFMPEGSAMPLE_BASICDATATYPE_H

#include <jni.h>
#include <android/log.h>
#include <sys/types.h>
#include "FLVData.h"


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


// H264
typedef enum {
    NALU_TYPE_SLICE    = 1,
    NALU_TYPE_DPA      = 2,
    NALU_TYPE_DPB      = 3,
    NALU_TYPE_DPC      = 4,
    NALU_TYPE_IDR      = 5,
    NALU_TYPE_SEI      = 6,
    NALU_TYPE_SPS      = 7,
    NALU_TYPE_PPS      = 8,
    NALU_TYPE_AUD      = 9,
    NALU_TYPE_EOSEQ    = 10,
    NALU_TYPE_EOSTREAM = 11,
    NALU_TYPE_FILL     = 12,
} NaluType;

typedef enum {
    NALU_PRIORITY_DISPOSABLE = 0,
    NALU_PRIRITY_LOW         = 1,
    NALU_PRIORITY_HIGH       = 2,
    NALU_PRIORITY_HIGHEST    = 3
} NaluPriority;


typedef struct
{
    int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
    unsigned max_size;            //! Nal Unit Buffer size
    int forbidden_bit;            //! should be always FALSE
    int nal_reference_idc;        //! NALU_PRIORITY_xxxx
    int nal_unit_type;            //! NALU_TYPE_xxxx
    char *buf;                    //! contains the first byte followed by the EBSP
} NALU_t;


// H265
typedef enum e_hevc_nalu_type {
    HEVC_NAL_TRAIL_N    = 0,
    HEVC_NAL_TRAIL_R    = 1,
    HEVC_NAL_TSA_N      = 2,
    HEVC_NAL_TSA_R      = 3,
    HEVC_NAL_STSA_N     = 4,
    HEVC_NAL_STSA_R     = 5,
    HEVC_NAL_RADL_N     = 6,
    HEVC_NAL_RADL_R     = 7,
    HEVC_NAL_RASL_N     = 8,
    HEVC_NAL_RASL_R     = 9,
    HEVC_NAL_VCL_N10    = 10,
    HEVC_NAL_VCL_R11    = 11,
    HEVC_NAL_VCL_N12    = 12,
    HEVC_NAL_VCL_R13    = 13,
    HEVC_NAL_VCL_N14    = 14,
    HEVC_NAL_VCL_R15    = 15,
    HEVC_NAL_BLA_W_LP   = 16,
    HEVC_NAL_BLA_W_RADL = 17,
    HEVC_NAL_BLA_N_LP   = 18,
    HEVC_NAL_IDR_W_RADL = 19,
    HEVC_NAL_IDR_N_LP   = 20,
    HEVC_NAL_CRA_NUT    = 21,
    HEVC_NAL_IRAP_VCL22 = 22,
    HEVC_NAL_IRAP_VCL23 = 23,
    HEVC_NAL_RSV_VCL24  = 24,
    HEVC_NAL_RSV_VCL25  = 25,
    HEVC_NAL_RSV_VCL26  = 26,
    HEVC_NAL_RSV_VCL27  = 27,
    HEVC_NAL_RSV_VCL28  = 28,
    HEVC_NAL_RSV_VCL29  = 29,
    HEVC_NAL_RSV_VCL30  = 30,
    HEVC_NAL_RSV_VCL31  = 31,
    HEVC_NAL_VPS        = 32,
    HEVC_NAL_SPS        = 33,
    HEVC_NAL_PPS        = 34,
    HEVC_NAL_AUD        = 35,
    HEVC_NAL_EOS_NUT    = 36,
    HEVC_NAL_EOB_NUT    = 37,
    HEVC_NAL_FD_NUT     = 38,
    HEVC_NAL_SEI_PREFIX = 39,
    HEVC_NAL_SEI_SUFFIX = 40
} E_HEVC_NALU_TYPE;

/********************************************************************************
typedef struct t_h264_nalu_header
{
    unsigned char forbidden_bit:1, nal_reference_idc:2, nal_unit_type:5;
} T_H264_NALU_HEADER; (1个字节, hevc header为2个字节)
*********************************************************************************/
typedef struct t_h265_nalu_header
{
    unsigned short forbidden_zero_bit:1, nal_unit_type:6, nuh_layer_id:6, nuh_temporal_id_plus1:3;
} T_H265_NALU_HEADER;

typedef struct t_h265_nalu
{
    int startCodeLen;

    T_H265_NALU_HEADER h265NaluHeader;

    unsigned int bodyLen;

    unsigned char *bodyData;
} T_H265_NALU;


// FLV
#define TAG_TYPE_SCRIPT 18
#define TAG_TYPE_AUDIO  8
#define TAG_TYPE_VIDEO  9

typedef unsigned char byte;
typedef struct {
    byte Signature[3];
    byte Version;
    byte Flags;
    uint DataOffset;
} FLV_HEADER;

typedef struct {
    byte TagType;
    byte DataSize[3];
    byte Timestamp[3];
    uint Reserved;
} TAG_HEADER;

#endif //FFMPEGSAMPLE_BASICDATATYPE_H
