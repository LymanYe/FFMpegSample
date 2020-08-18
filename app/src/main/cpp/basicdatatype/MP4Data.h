//
// Created by Lyman on 2020/8/17.
//

#ifndef FFMPEGSAMPLE_MP4DATA_H
#define FFMPEGSAMPLE_MP4DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../base/Base.h"

#define PRINTF_MP4_DEBUG

#define BOX_TYPE_FTYPE "ftyp"
#define BOX_TYPE_MOOV "moov"
#define BOX_TYPE_MVHD "mvhd"
#define BOX_TYPE_TRAK "trak"
#define BOX_TYPE_TKHD "tkhd"
#define BOX_TYPE_EDTS "edts"
#define BOX_TYPE_MDIA "mdia"
#define BOX_TYPE_MDHD "mdhd"
#define BOX_TYPE_HDLR "hdlr"
#define BOX_TYPE_MINF "minf"
#define BOX_TYPE_VMHD "vmhd"
#define BOX_TYPE_DINF "dinf"
#define BOX_TYPE_DREF "dref"
#define BOX_TYPE_STBL "stbl"
#define BOX_TYPE_STSD "stsd"
#define BOX_TYPE_STTS "stts"
#define BOX_TYPE_STSS "stss"
#define BOX_TYPE_STSC "stsc"
#define BOX_TYPE_STSZ "stsz"
#define BOX_TYPE_STCO "stco"
#define BOX_TYPE_UDTA "udta"

#define MAX_BOX_SIZE_LEN 4
#define MAX_BOX_TYPE_LEN 4
#define MAX_HANDLER_TYPE_LEN 4
#define MAX_FTYP_BRABDS_LEN 4
#define MAX_FTYP_BRABDS_NUM 4
#define MAX_STTS_ENTRY_NUM 8
#define MAX_STSS_ENTRY_NUM 8
#define MAX_STSC_ENTRY_NUM 100
#define MAX_STSZ_ENTRY_NUM 100 /* now parse 100 frame */
#define MAX_STCO_ENTRY_NUM 100
#define MAX_MVHD_RESERVED_LEN 10
#define MAX_PRE_DEFINE_LEN 24
#define MAX_MATRIX_LEN 36
#define MAX_HDLR_NAME_LEN 100
#define MP4TAG "MP4Data"


typedef struct t_box_header
{
    int boxSize;

    unsigned char boxType[MAX_BOX_TYPE_LEN+1];

    long largeBoxSize; /* if boxSize=1 use, if boxSize=0, end of file */
} T_BOX_HEADER;

/********************************************************************************************
**                            File Type Box (ftyp): file type, 表明文件类型
**
--------------------------------------------------------------------------------------------
**        字段名称            　　|    长度(bytes)   |        有关描述
--------------------------------------------------------------------------------------------
**        boxsize               |    4            |        box的长度
**        boxtype               |    4            |        box的类型
**        major_brand           |    4            |
**        minor_version         |    4            |        版本号
**        compatible_brands     |    4 * N        |        本文件遵从的多种协议(ismo, iso2, mp41)
********************************************************************************************/
typedef struct t_box4ftyp_brand
{
    unsigned char brands[MAX_FTYP_BRABDS_LEN+1];
} T_BOX4FTYP_BRAN;

typedef struct t_box4ftyp
{
    unsigned char major_brand[MAX_FTYP_BRABDS_LEN+1];

    int minor_version;

    T_BOX4FTYP_BRAN compatible_brands[MAX_FTYP_BRABDS_NUM];
} T_BOX4FTYP;

/************************************************************************************************************
**                                            mvhd: movie header, 文件的总体信息: 时长, 创建时间等
**
--------------------------------------------------------------------------------------------
**        字段名称            　　|    长度(bytes)   |        有关描述
--------------------------------------------------------------------------------------------
**        boxsize               |    4            |        box的长度
**        boxtype               |    4            |        box的类型
**        version               |    1            |        box版本，0或1，一般为0（以下字节数均按version = 0）
**        flags                 |    3            |
**        creation time         |    4            |        创建时间（相对于UTC时间1904 - 01 - 01零点的秒数）
**        modification time     |    4            |        修改时间
**        time scale            |    4            |        文件媒体在1秒时间内的刻度值，可以理解为1秒长度的时间单元数
**        duration              |    4            |        该track的时间长度，用duration和time scale值可以计算track时长
**        rate                  |    4            |        推荐播放速率，高16位和低16位分别为小数点整数部分和小数部分，即[16.16] 格式.该值为1.0 (0x00010000)
**        volume                |    2            |        与rate类似，[8.8] 格式，1.0（0x0100）表示最大音量
**        reserved              |    10           |        保留位
**        matrix                |    36           |        视频变换矩阵
**        pre-defined           |    24           |
**        next track id         |    4            |        下一个track使用的id号
**
if (version==1)
{
    unsigned int(64) creation_time;
    unsigned int(64) modification_time;
    unsigned int(32) timescale;
    unsigned int(64) duration;
}
else
{
    unsigned int(32) creation_time;
    unsigned int(32) modification_time;
    unsigned int(32) timescale;
    unsigned int(32) duration;
}
************************************************************************************************************/
typedef struct t_box4mvhd
{
    int creation_time;
    int modification_time;
    int timescale;
    int duration;
    float rate;
    float volume;
    int next_track_id;
} T_BOX4MVHD;

/************************************************************************************************************
**                                        tkhd: track header, track的总体信息, 如时长, 宽高等
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)   |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4            |        box的长度
**        boxtype                |    4            |        box的类型
**        version                |    1            |        box版本，0或1，一般为0。（以下字节数均按version = 0）
**        flags                  |    3            |        按位或操作结果值，预定义如下;
                                                    　　　　 0x000001 track_enabled，否则该track不被播放；
                                                    　　　　 0x000002 track_in_movie，表示该track在播放中被引用；
                                                    　　　　 0x000004 track_in_preview，表示该track在预览时被引用。
                                                    　　　　 一般该值为7，如果一个媒体所有track均未设置track_in_movie和track_in_preview,将被理解为所有track均设置了这两项;
                                                    　　　　 对于hint track，该值为0;
**        creation_time          |    4            |        创建时间（相对于UTC时间1904 - 01 - 01零点的秒数）
**        modification_time      |    4            |        修改时间
**        track_id               |    4            |        id号 不能重复且不能为0
**        reserved               |    4            |        保留位
**        duration               |    4            |        track的时间长度
**        reserved               |    8            |        保留位
**        layer                  |    2            |        视频层，默认为0，值小的在上层
**        alternate_group        |    2            |        track分组信息，默认为0表示该track未与其他track有群组关系
**        volume                 |    2            |        [8.8] 格式，如果为音频track，1.0（0x0100）表示最大音量；否则为0
**        reserved               |    2            |        保留位
**        matrix                 |    36           |        视频变换矩阵
**        width                  |    4            |        宽
**        height                 |    4            |        高，均为[16.16] 格式值 与sample描述中的实际画面大小比值，用于播放时的展示宽高
if (version==1)
{
    unsigned int(64) creation_time;
    unsigned int(64) modification_time;
    unsigned int(32) track_ID;
    const unsigned int(32) reserved = 0;
    unsigned int(64) duration;
}
else
{
    unsigned int(32) creation_time;
    unsigned int(32) modification_time;
    unsigned int(32) track_ID;
    const unsigned int(32) reserved = 0;
    unsigned int(32) duration;
}
************************************************************************************************************/
typedef struct t_box4tkhd
{
    int flags;
    int creation_time;
    int modification_time;
    int track_id;
    int duration;
    int layer;
    int alternate_group;
    float volume;
    float width;
    float height;
} T_BOX4TKHD;

/************************************************************************************************************
**                                        mdhd: 包含了了该track的总体信息, mdhd和tkhd 内容大致都是一样的.
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　|　　    长度(bytes)　　　|        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize               |    4            　　  |        box的长度
**        boxtype               |    4            　　  |        box的类型
**        version               |    1　　　　　　　　　|        box版本0或1 一般为0 (以下字节数均按version=0)
**        flags                 |    3            　　  |
**        creation_time         |    4            　　  |        创建时间（相对于UTC时间1904 - 01 - 01零点的秒数）
**        modification_time     |    4            　　  |        修改时间
**        time_scale            |    4            　　  |
**        duration              |    4            　　　|        track的时间长度
**        language              |    2            　　　|        媒体语言码,最高位为0 后面15位为3个字符[见ISO 639-2/T标准中定义]
**        pre-defined           |    2            　　  |        保留位

** tkhd通常是对指定的track设定相关属性和内容, 而mdhd是针对于独立的media来设置的, 一般情况下二者相同.
************************************************************************************************************/
typedef struct t_box4mdhd
{
    int creation_time;
    int modification_time;
    int timescale;
    int duration;
    short language;
} T_BOX4MDHD;

/************************************************************************************************************
**                                        hdlr: Handler Reference Box, 媒体的播放过程信息, 该box也可以被包含在meta box(meta)中
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本0或1 一般为0 (以下字节数均按version=0)
**        flags                  |    3             |
**        pre-defined            |    4             |
**        handler type           |    4             |        在media box中，该值为4个字符
                                                    　　　　　　"vide"— video track
                                                    　　　　　　"soun"— audio track
                                                    　　　　　　"hint"— hint track
**        reserved               |    12            |
**        name                   |    不定           |        track type name，以‘\0’结尾的字符串
************************************************************************************************************/
typedef struct t_box4hdlr
{
    unsigned char handler_type[MAX_HANDLER_TYPE_LEN+1];
    unsigned char name[MAX_HDLR_NAME_LEN+1];
} T_BOX4HDLR;

/************************************************************************************************************
**                                        vmhd: Video Media Header Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4            |        box的长度
**        boxtype                |    4            |        box的类型
**        version                |    1            |        box版本0或1 一般为0 (以下字节数均按version=0)
**        flags                     |    3            |
**        graphics_mode          |    4            |        视频合成模式，为0时拷贝原始图像，否则与opcolor进行合成
**        opcolor                |    2 ×3         |        ｛red，green，blue｝

"vide"—vmhd 视频
"soun"— smhd 音频
"hint"—hmhd 忽略
************************************************************************************************************/
typedef struct t_box4vmhd
{
    int graphics_mode;
} T_BOX4VMHD;

/************************************************************************************************************
**                                        dref: data reference box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本0或1 一般为0 (以下字节数均按version=0)
**        flags                  |    3             |
**        entry count            |    4             |         "url"或"urn"表的元素个数
**        "url"或"urn"列表       |    不定          |

** "dref"下会包含若干个"url"或"urn", 这些box组成一个表, 用来定位track数据. 简单的说, track可以被分成若干段,
   每一段都可以根据"url"或"urn"指向的地址来获取数据, sample描述中会用这些片段的序号将这些片段组成一个完整的track.
   一般情况下, 当数据被完全包含在文件中时, "url"或"urn"中的定位字符串是空的.
************************************************************************************************************/
typedef struct t_box4dref
{
    int entry_count;
} T_BOX4DREF;

/************************************************************************************************************
**                                        stsd: Sample Description Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本0或1 一般为0 (以下字节数均按version=0)
**        entry count            |    4             |         "url"或"urn"表的元素个数

** box header和version字段后会有一个entry count字段, 根据entry的个数, 每个entry会有type信息, 如"vide", "sund"等,
   根据type不同sample description会提供不同的信息, 例如对于video track, 会有"VisualSampleEntry"类型信息,
   对于audio track会有"AudioSampleEntry"类型信息. 视频的编码类型, 宽高, 长度, 音频的声道, 采样等信息都会出现在这个box中
************************************************************************************************************/
typedef struct t_box4stsd
{
    int entry_count;

    //TODO
} T_BOX4STSD;

/************************************************************************************************************
**                                        stts: Time To Sample Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本，0或1，一般为0（以下字节数均按version = 0）
**        flags                  |    3             |
**        entry count            |    4             |         sample_count和sample_delta的个数
**        sample_count           |    4             |
**        sample_delta           |    4             |

** "stts”"存储了sample的duration, 描述了sample时序的映射方法, 我们通过它可以找到任何时间的sample. "stts"可以
   包含一个压缩的表来映射时间和sample序号, 用其他的表来提供每个sample的长度和指针. 表中每个条目提供了在同一个
   时间偏移量里面连续的sample序号, 以及samples的偏移量. 递增这些偏移量, 就可以建立一个完整的time to sample表.

   例: 说明该视频包含87帧数据(sample_count), 每帧包含512个采样(sample_delta). 总共512*87=44544个采样,
       和我们前面mdhd box的Duration完全一致。
       Duration/TimeScale = 44544/12288 = 3.625s, 正是我们的视频播放长度.
       12288/512 = 24 p/s (帧率)
************************************************************************************************************/
typedef struct t_box4stts_entry
{
    int sample_count;
    int sample_delta;
} T_BOX4STTS_ENTRY;

typedef struct t_box4stts
{
    int entry_count;

    T_BOX4STTS_ENTRY entrys[MAX_STTS_ENTRY_NUM];
} T_BOX4STTS;

/************************************************************************************************************
**                                        stss: Sync Sample Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本，0或1，一般为0（以下字节数均按version = 0）
**        flags                  |    3             |
**        entry count            |    4             |         sample_num的个数
**        sample_num                |    4             |

** "stss"确定media中的关键帧. 对于压缩媒体数据, 关键帧是一系列压缩序列的开始帧, 其解压缩时不依赖以前的帧,
   而后续帧的解压缩将依赖于这个关键帧. "stss"可以非常紧凑的标记媒体内的随机存取点, 它包含一个sample序号表,
   表内的每一项严格按照sample的序号排列, 说明了媒体中的哪一个sample是关键帧. 如果此表不存在, 说明每一个sample
   都是一个关键帧, 是一个随机存取点.
************************************************************************************************************/
typedef struct t_box4stss_entry
{
    int sample_num;
} T_BOX4STSS_ENTRY;

typedef struct t_box4stss
{
    int entry_count;

    T_BOX4STSS_ENTRY entrys[MAX_STSS_ENTRY_NUM];
} T_BOX4STSS;

/************************************************************************************************************
**                                        stsc: Sample To Chunk Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本，0或1，一般为0（以下字节数均按version = 0）
**        flags                  |    3             |
**        entry count            |    4             |         entry的个数
**        first_chunk            |    4             |
**        samples_per_chunk      |    4             |
**        sample_des_index       |    4             |

** 用chunk组织sample可以方便优化数据获取, 一个thunk包含一个或多个sample. "stsc"中用一个表描述了sample与chunk的映射关系,
   查看这张表就可以找到包含指定sample的thunk, 从而找到这个sample.
************************************************************************************************************/
typedef struct t_box4stsc_entry
{
    int first_chunk;
    int samples_per_chunk;
    int sample_description_index;
} T_BOX4STSC_ENTRY;

typedef struct t_box4stsc
{
    int entry_count;

    T_BOX4STSC_ENTRY entrys[MAX_STSC_ENTRY_NUM];
} T_BOX4STSC;

/************************************************************************************************************
**                                        stsz: Sample To Chunk Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本，0或1，一般为0（以下字节数均按version = 0）
**        flags                  |    3             |
**        sample_size            |    4             |
**        sample_count           |    4             |         entry的个数
**        entry_size             |    4             |

**  "stsz"定义了每个sample的大小, 包含了媒体中全部sample的数目和一张给出每个sample大小的表. 这个box相对来说体积是比较大的.
************************************************************************************************************/
typedef struct t_box4stsz_entry
{
    int entry_size;
} T_BOX4STSZ_ENTRY;

typedef struct t_box4stsz
{
    int sample_size;
    int sample_count;

    T_BOX4STSZ_ENTRY entrys[MAX_STSZ_ENTRY_NUM];
} T_BOX4STSZ;

/************************************************************************************************************
**                                        stco: Chunk Offset Box
**
-------------------------------------------------------------------------------------------------------------
**        字段名称            　　 |    长度(bytes)    |        有关描述
-------------------------------------------------------------------------------------------------------------
**        boxsize                |    4             |        box的长度
**        boxtype                |    4             |        box的类型
**        version                |    1             |        box版本，0或1，一般为0（以下字节数均按version = 0）
**        flags                  |    3             |
**        entry_count            |    4             |
**        chunk_offset           |    4             |

**  "stco"定义了每个thunk在媒体流中的位置, sample的偏移可以根据其他box推算出来. 位置有两种可能, 32位的和64位的,
    后者对非常大的电影很有用. 在一个表中只会有一种可能, 这个位置是在整个文件中的, 而不是在任何box中的.
    这样做就可以直接在文件中找到媒体数据, 而不用解释box. 需要注意的是一旦前面的box有了任何改变, 这张表都要重新建立, 因为位置信息已经改变了.
************************************************************************************************************/
typedef struct t_box4stco_entry
{
    int chunk_offset;
} T_BOX4STCO_ENTRY;

typedef struct t_box4stco
{
    int entry_count;

    T_BOX4STCO_ENTRY entrys[MAX_STCO_ENTRY_NUM];
} T_BOX4STCO;

typedef struct t_box
{
    T_BOX_HEADER boxHeader;

    unsigned char *boxData;
} T_BOX;


void DealBox4ftyp(const T_BOX *box);

void DealBox4mvhd(const unsigned char *mvhdData);

void DealBox4tkhd(const unsigned char *tkhdData);

void DealBox4dref(const T_BOX *box);

void DealBox4dinf(const T_BOX *box);

void DealBox4stts(const unsigned char *sttsData);

void DealBox4stss(const unsigned char *stssData);

void DealBox4stsc(const unsigned char *stscData);

void DealBox4stsz(const unsigned char *stszData);

void DealBox4stco(const unsigned char *stcoData);

void DealBox4stbl(const T_BOX *box);

void DealBox4mdhd(const unsigned char *mdhdData);

void DealBox4hdlr(const unsigned char *hdlrData);

void DealBox4vmdhd(const unsigned char *vmdhdData);

void DealBox4minf(const T_BOX *box);

void DealBox4mdia(const T_BOX *box);

void DealBox4trak(const T_BOX *box);

void DealBox4moov(const T_BOX *box);

void DealBox(const T_BOX *box);
#endif //FFMPEGSAMPLE_MP4DATA_H
