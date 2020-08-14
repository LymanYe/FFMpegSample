//
// Created by Lyman on 2020/8/14.
//

#ifndef FFMPEGSAMPLE_FLVDATA_H
#define FFMPEGSAMPLE_FLVDATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <jni.h>
#include <android/log.h>
#define TAB44 "    "
#define PRINTF_DEBUG
#define FLVTAG "FLVData"

#define MAX_SIGNATURE_LEN 3
#define MAX_PRE_TAG_SIZE_LEN 4
#define MIN_FLV_HEADER_LEN 9
#define MAX_TAG_HEADER_LEN 11
#define MAX_PARSE_TAG_NUM 15
#define MAX_AMF_STR_SIZE 255
#define LOGD(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)

/************************************************************************************************************
 **                                        flv header: 记录了flv的类型, 版本等信息, 是flv的开头, 一般都差不多, 占9bytes
 **
 -------------------------------------------------------------------------------------------------------------
 **        字段名称            　　 |    长度(bytes)    |        有关描述
 -------------------------------------------------------------------------------------------------------------
 **        signature              |    3             |        文件标识, 总是为"FLV", 0x46 0x4c 0x56
 **        version                |    1             |        版本(目前为0x01)
 **        flag                     |    3             |          文件的标志位说明. 前5位保留, 必须为0;
                                                              第6位为音频Tag: 1表示有音频; 第七位保留, 为0; 第8位为视频Tag: 1表示有视频
 **        headersize             |    4             |        整个header的长度, 一般为9(版本为0x01时); 大于9表示下面还有扩展信息
 ************************************************************************************************************/
/*
   1. unsigned char reserved5: 5, flags_audio: 1, reserved1: 1, flags_video: 1;
   2. unsigned char : 5, flags_audio: 1, : 1, flags_video: 1; (无名说明无法使用, 仅占位)
   3. 下面结构体位域的另外两种写法.
*/
typedef struct t_flv_header
{
    unsigned char signature[MAX_SIGNATURE_LEN+1];
    unsigned char version;
    unsigned char : 5;
    unsigned char flags_audio: 1;
    unsigned char : 1;
    unsigned char flags_video: 1;

    int headersize;
} T_FLV_HEADER;

/************************************************************************************************************
**                                        tag header
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        type                   |    1             |        数据类型, (0x12)为脚本类型; (0x08)为音频类型; (0x09）为视频类型
**        data_size              |    3             |        数据区长度
**        timestamp                 |    3             |          时间戳, 类型为(0x12)的tag时间戳一直为0, (0xFFFFFF)可以表示长度为4小时, 单位为毫秒.
**        timestamp_extended     |    1             |        将时间戳扩展为4bytes, 代表高8位, 一般都为0, 长度为4小时的flv一般很少见了
**        streamid               |    3             |        总为0
************************************************************************************************************/
typedef struct t_flv_tag_header
{
    int type;
    int data_size;
    int timestamp;
    int timestamp_extended;
    int streamid;
} T_FLV_TAG_HEADER;

/************************************************************************************************************
**                                        video tag header
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        FreameType             |    4(bits)             |  FrameType为数据类型, 1为关键帧, 2为非关键帧, 3为h263的非关键帧,
                                                             4为服务器生成关键帧, 5为视频信息或命令帧.
**        CodecId                |    4(bits)             |  CodecID为包装类型, 1为JPEG, 2为H263, 3为Screen video,
                                                             4为On2 VP6, 5为On2 VP6, 6为Screen videoversion 2, 7为AVC

CodecID=2, 为H263VideoPacket;
CodecID=3, 为ScreenVideopacket;
CodecID=4, 为VP6FLVVideoPacket;
CodecID=5, 为VP6FLVAlphaVideoPacket;
CodecID=6, 为ScreenV2VideoPacket;
CodecID=7, 为AVCVideoPacket.
************************************************************************************************************/
typedef struct t_flv_tag_video_header
{
    unsigned char freameType:4, codecId:4;
} T_FLV_TAG_VIDEO_HEADER;

/************************************************************************************************************
**                                        AVCDecoderConfigurationRecord
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        configurationVersion   |    1             |        配置版本占用8位, 一定为1
**        AVCProfileIndication   |    1             |        profile_idc占用8位, 从H.264标准SPS第一个字段profile_idc拷贝而来, 指明所用profile
**        profile_compatibility  |    1             |        占用8位, 从H.264标准SPS拷贝的冗余字
**        AVCLevelIndication     |    1             |        level_idc占用8位, 从H.264标准SPS第一个字段level_idc拷贝而来, 指明所用 level
**        reserved               |    6b            |        保留位占6位, 值一定为'111111'
**        lengthSizeMinusOne     |    2b            |        占用2位, 表示NAL单元头的长度, 0表示1字节, 1表示2字节, 2表示3字节, 3表示4字节
**        reserved               |    3b            |        保留位占3位, 值一定为'111'
**        numOfSPS               |    5b            |        numOfSequenceParameterSets占用5位, 表示当前SPS的个数
**        SPSLength               |    2             |        sequenceParameterSetLength占用16位, SPS占用的长度
**        SPSData                   |    *             |
**        numOfPPS               |    5b            |        numOfPictureParameterSets占用8位, 表示当前PPS的个数
**        PPSLength               |    2             |        pictureParameterSetLength占用16位, PPS占用的长度
**        PPSData                    |    *             |        numOfPictureParameterSets占用8位, 表示当前PPS的个数

AVCProfileIndication, profile_compatibility, AVCLevelIndication就是拷贝SPS的前3个字节
************************************************************************************************************/
typedef struct t_flv_tag_avc_dec_cfg
{
    unsigned char configurationVersion;
    unsigned char AVCProfileIndication;
    unsigned char profile_compatibility;
    unsigned char AVCLevelIndication;
    unsigned char :6, lengthSizeMinusOne:2;

    unsigned char :3, numOfSequenceParameterSets:5;
    unsigned short spsLen;
    unsigned char *spsData;

    unsigned char numOfPictureParameterSets;
    unsigned short ppsLen;
    unsigned char *ppsData;
} T_FLV_TAG_AVC_DEC_CFG;

/************************************************************************************************************
**                                        avc video packet header
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        AVCPacketType占用1字节 |    1                |
**        CompositionTime        |    3             |

AVCVideoPacket同样包括Packet Header和Packet Body两部分：
Packet Header:
        AVCPacketType占用1字节, 仅在AVC时有此字段
               0, AVC sequence header (SPS、PPS信息等)
               1, AVC NALU
               2, AVC end of sequence (lower level NALU sequence ender is not required or supported)

        CompositionTime占用24位, 相对时间戳, 如果AVCPacketType=0x01为相对时间戳; 其它, 均为0;
        该值表示当前帧的显示时间, tag的时间为解码时间, 显示时间等于 解码时间+CompositionTime.
************************************************************************************************************/
typedef struct t_flv_tag_avc_video_packet
{
    unsigned char avcPacketType;

    int compositionTime;

    union videoPacket
    {
        T_FLV_TAG_AVC_DEC_CFG avcDecCfg;
    } vp;
} T_FLV_TAG_AVC_VIDEO_PACKET;

typedef struct t_flv_tag_audio_header
{
    unsigned char soundFormat:4, soundRate:2, soundSize:1, soundType:1;
} T_FLV_TAG_AUDIO_HEADER;

typedef struct t_flv_tag_aac_spec_cfg
{
    unsigned char audioObjectType:5;
    unsigned char samplingFreqIndex:4, channelCfg:2;
} T_FLV_TAG_AAC_SPEC_CFG;

typedef struct t_flv_tag_aac_audio_packet
{
    unsigned char aacPacketType;

    union audioPacket
    {
        T_FLV_TAG_AAC_SPEC_CFG aacSpecCfg;
    } ap;
} T_FLV_TAG_AAC_AUDIO_PACKET;

typedef struct t_flv_tag
{
} T_FLV_TAG;

double dealAmfNumber(unsigned char *amfNum);

void DealFlvHeader(unsigned char* const headerData, FILE *logFile);

void DealTagHeader(unsigned char* const headerData, T_FLV_TAG_HEADER *tagHeader, FILE *logFile);

void DealScriptTagData(unsigned char* const tagData, unsigned int tagDataSize);

void DealVideoTagData(unsigned char* const tagData);

void DealAudioTagData(unsigned char* const tagData);

void DealTagData(unsigned char* const tagData, const int tagType, const unsigned int tagSize);
#endif //FFMPEGSAMPLE_FLVDATA_H
