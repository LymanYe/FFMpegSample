//
// Created by Lyman on 2020/8/17.
//

#include "MP4Data.h"

void DealBox4ftyp(const T_BOX *box)
{
    int i = 0;
    int j = 0;
    int brandsNum = 0;

    T_BOX4FTYP box4ftyp = {0};

    memset(&box4ftyp, 0x0, sizeof(T_BOX4FTYP));

    memcpy(box4ftyp.major_brand, box->boxData, 4);
    box4ftyp.major_brand[MAX_FTYP_BRABDS_LEN] = '\0';

    box4ftyp.minor_version =  box->boxData[4] << 24 | box->boxData[5] << 16 | box->boxData[6] << 8 | box->boxData[7];

    brandsNum = (box->boxHeader.boxSize - MAX_BOX_SIZE_LEN - MAX_BOX_TYPE_LEN - MAX_FTYP_BRABDS_LEN - 4) / 4;

    /* 1. if not have '\0', 每个brands的内存是连续的, 导致打印时后面的每4个数据都会加到前面;
       2. unsigned char brands[MAX_FTYP_BRABDS_LEN+1]; 可解决, 此时也不必加'\0', 但需初始化;
       3. 因此字符串最好定义+1并赋'\0';
       4. 复现: unsigned char brands[MAX_FTYP_BRABDS_LEN]
    */
    for (i=0; i<brandsNum; i++)
    {
        memcpy(box4ftyp.compatible_brands[i].brands, box->boxData+MAX_FTYP_BRABDS_LEN+4+4*i, 4);

        box4ftyp.compatible_brands[i].brands[MAX_FTYP_BRABDS_LEN] = '\0';
    }

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\tmajor_brand: %s, minor_version: %d, compatible_brands: ", box4ftyp.major_brand, box4ftyp.minor_version);

    for (i=0; i<brandsNum; i++)
    {
        if (i==brandsNum-1)
        {
            LOGD(MP4TAG, "%s", box4ftyp.compatible_brands[i].brands);
        }
        else
        {
            LOGD(MP4TAG, "%s,", box4ftyp.compatible_brands[i].brands);
        }
    }

    LOGD(MP4TAG, "\n");
#endif
}

void DealBox4mvhd(const unsigned char *mvhdData)
{
    unsigned char *data = NULL;

    T_BOX4MVHD box4mvhd = {0};

    memset(&box4mvhd, 0x0, sizeof(T_BOX4MVHD));

    data = (unsigned char *)mvhdData;

    data += 4;
    box4mvhd.creation_time = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mvhd.modification_time = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mvhd.timescale = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mvhd.duration = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    //box4mvhd.rate = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    box4mvhd.rate = (data[0] << 8 | data[1]) + (data[2] << 8 | data[3]);

    data += 4;
    //box4mvhd.volume = data[0] << 8 | data[1];
    box4mvhd.volume = data[0] + data[1];

    data += 2;
    data += (MAX_MVHD_RESERVED_LEN + MAX_PRE_DEFINE_LEN + MAX_MATRIX_LEN);
    box4mvhd.next_track_id = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\tcreation_time: %d, modification_time: %d, timescale: %d, duration: %d, rate: %f, volume: %f, next_track_id: %d\n",
           box4mvhd.creation_time, box4mvhd.modification_time, box4mvhd.timescale, box4mvhd.duration, box4mvhd.rate, box4mvhd.volume, box4mvhd.next_track_id);
#endif
}

void DealBox4tkhd(const unsigned char *tkhdData)
{
    unsigned char *data = NULL;

    T_BOX4TKHD box4tkhd = {0};

    memset(&box4tkhd, 0x0, sizeof(box4tkhd));

    data = (unsigned char *)tkhdData;

    box4tkhd.flags = data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4tkhd.creation_time = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4tkhd.modification_time = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4tkhd.track_id = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    data += 4; /* 4 reserved */
    box4tkhd.duration = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    data += 8; /* 8 reserved */
    box4tkhd.layer = data[0] << 8 | data[1];

    data += 2;
    box4tkhd.alternate_group = data[0] << 8 | data[1];

    data += 2;
    box4tkhd.volume = data[0] + data[1];

    data += 2;

    data += 2;

    data += 36;
    box4tkhd.width = (data[0] << 8 | data[1]) + (data[2] << 8 | data[3]);

    data += 4;
    box4tkhd.height = (data[0] << 8 | data[1]) + (data[2] << 8 | data[3]);

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\tflags: %d, creation_time: %d, modification_time: %d, track_id: %d, duration: %d, layer: %d, alternate_group: %d, volume: %f, width: %f, height: %f\n",
           box4tkhd.flags, box4tkhd.creation_time, box4tkhd.modification_time, box4tkhd.track_id, box4tkhd.duration, box4tkhd.layer, box4tkhd.alternate_group, box4tkhd.volume, box4tkhd.width, box4tkhd.height);
#endif
}

void DealBox4dref(const T_BOX *box)
{
    // TODO
}

void DealBox4dinf(const T_BOX *box)
{    int boxSize = 0;
    int dinfDataSize = 0;

    unsigned char *dinfData = NULL;
    unsigned char *data = NULL;

    char boxType[MAX_BOX_TYPE_LEN+1] = {0};

    T_BOX drefBox = {0};

    dinfData = box->boxData;
    dinfDataSize = box->boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN;

    while (dinfDataSize > 0)
    {
        boxSize = dinfData[0] << 24 | dinfData[1] << 16 | dinfData[2] << 8 | dinfData[3];

        memcpy(boxType, dinfData+MAX_BOX_SIZE_LEN, 4);

#ifdef PRINTF_MP4_DEBUG
        LOGD(MP4TAG, "\t\t\t\t\t****BOX: Layer6****\n");
        LOGD(MP4TAG, "\t\t\t\t\t\tsize: %d\n", boxSize);
        LOGD(MP4TAG, "\t\t\t\t\t\ttype: %s\n", boxType);
#endif
        if (0 == strcmp(boxType, BOX_TYPE_DREF))
        {
            memset(&drefBox, 0x0, sizeof(T_BOX));

            drefBox.boxHeader.boxSize = boxSize;

            memcpy(drefBox.boxHeader.boxType, boxType, strlen(boxType));

            drefBox.boxData = (unsigned char*)malloc(boxSize);
            if (drefBox.boxData)
            {
                memcpy(drefBox.boxData, dinfData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4dref((const T_BOX*)&drefBox);

                free(drefBox.boxData);
                drefBox.boxData = NULL;
            }
        }

        dinfData += boxSize;
        dinfDataSize -= boxSize;
    }
}

void DealBox4stts(const unsigned char *sttsData)
{
    int i = 0;

    unsigned char *data = NULL;

    T_BOX4STTS box4stts = {0};

    memset(&box4stts, 0x0, sizeof(box4stts));

    data = (unsigned char *)sttsData;

    data += 4;

    box4stts.entry_count = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    for (i=0; i<box4stts.entry_count; i++)
    {
        if (i == MAX_STTS_ENTRY_NUM)
        {
            break;
        }

        box4stts.entrys[i].sample_count = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;

        box4stts.entrys[i].sample_delta = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;
    }

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\tentry_count: %d, [sample_count, sample_delta]: ", box4stts.entry_count);

    if (box4stts.entry_count>MAX_STTS_ENTRY_NUM)
    {
        box4stts.entry_count = MAX_STTS_ENTRY_NUM;
    }

    for (i=0; i<box4stts.entry_count; i++)
    {
        if (i>0)
        {
            LOGD(MP4TAG, ", ");
        }

        LOGD(MP4TAG, "[%d, %d]", box4stts.entrys[i].sample_count, box4stts.entrys[i].sample_delta);
    }

    if (box4stts.entry_count==MAX_STTS_ENTRY_NUM)
    {
        LOGD(MP4TAG, "...(just show %d now)", MAX_STTS_ENTRY_NUM);
    }

    LOGD(MP4TAG, "\n");
#endif
}

void DealBox4stss(const unsigned char *stssData)
{
    int i = 0;

    unsigned char *data = NULL;

    T_BOX4STSS box4stss = {0};

    memset(&box4stss, 0x0, sizeof(box4stss));

    data = (unsigned char *)stssData;

    data += 4;

    box4stss.entry_count = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    for (i=0; i<box4stss.entry_count; i++)
    {
        if (i == MAX_STSS_ENTRY_NUM)
        {
            break;
        }

        box4stss.entrys[i].sample_num = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;
    }

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\tentry_count: %d, sample_num: ", box4stss.entry_count);

    if (box4stss.entry_count>MAX_STSS_ENTRY_NUM)
    {
        box4stss.entry_count = MAX_STSS_ENTRY_NUM;
    }

    for (i=0; i<box4stss.entry_count; i++)
    {
        if (i>0)
        {
            LOGD(MP4TAG, ", ");
        }

        LOGD(MP4TAG, "%d", box4stss.entrys[i].sample_num);
    }

    if (box4stss.entry_count==MAX_STSS_ENTRY_NUM)
    {
        LOGD(MP4TAG, "...(just show %d now)", MAX_STSS_ENTRY_NUM);
    }

    LOGD(MP4TAG, "\n");
#endif
}

void DealBox4stsc(const unsigned char *stscData)
{
    int i = 0;

    unsigned char *data = NULL;

    T_BOX4STSC box4stsc = {0};

    memset(&box4stsc, 0x0, sizeof(box4stsc));

    data = (unsigned char *)stscData;

    data += 4;

    box4stsc.entry_count = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    for (i=0; i<box4stsc.entry_count; i++)
    {
        if (i == MAX_STSC_ENTRY_NUM)
        {
            break;
        }

        box4stsc.entrys[i].first_chunk = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;

        box4stsc.entrys[i].samples_per_chunk = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;

        box4stsc.entrys[i].sample_description_index = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;
    }

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\tentry_count: %d, [first_chunk, samples_per_chunk, sample_description_index]: ", box4stsc.entry_count);

    if (box4stsc.entry_count>MAX_STSC_ENTRY_NUM)
    {
        box4stsc.entry_count = MAX_STSC_ENTRY_NUM;
    }

    for (i=0; i<box4stsc.entry_count; i++)
    {
        if (i>0)
        {
            LOGD(MP4TAG, ", ");
        }

        LOGD(MP4TAG, "[%d, %d, %d]", box4stsc.entrys[i].first_chunk, box4stsc.entrys[i].samples_per_chunk, box4stsc.entrys[i].sample_description_index);
    }

    if (box4stsc.entry_count==MAX_STSC_ENTRY_NUM)
    {
        LOGD(MP4TAG, "...(just show %d now)", MAX_STSC_ENTRY_NUM);
    }

    LOGD(MP4TAG, "\n");
#endif
}

void DealBox4stsz(const unsigned char *stszData)
{
    int i = 0;

    unsigned char *data = NULL;

    T_BOX4STSZ box4stsz = {0};

    memset(&box4stsz, 0x0, sizeof(box4stsz));

    data = (unsigned char *)stszData;

    data += 4;

    box4stsz.sample_size = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    box4stsz.sample_count = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    for (i=0; i<box4stsz.sample_count; i++)
    {
        if (i == MAX_STSZ_ENTRY_NUM)
        {
            break;
        }

        box4stsz.entrys[i].entry_size = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;
    }

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\tsample_size: %d, sample_count: %d, [entry_size]: ", box4stsz.sample_size, box4stsz.sample_count);

    if (box4stsz.sample_count>MAX_STSZ_ENTRY_NUM)
    {
        box4stsz.sample_count = MAX_STSZ_ENTRY_NUM;
    }

    for (i=0; i<box4stsz.sample_count; i++)
    {
        if (i>0)
        {
            LOGD(MP4TAG, ", ");
        }

        LOGD(MP4TAG, "[%d]", box4stsz.entrys[i].entry_size);
    }

    if (box4stsz.sample_count==MAX_STSZ_ENTRY_NUM)
    {
        LOGD(MP4TAG, "...(just show %d now)", MAX_STSZ_ENTRY_NUM);
    }

    LOGD(MP4TAG, "\n");
#endif
}

void DealBox4stco(const unsigned char *stcoData)
{
    int i = 0;

    unsigned char *data = NULL;

    T_BOX4STCO box4stco = {0};

    memset(&box4stco, 0x0, sizeof(box4stco));

    data = (unsigned char *)stcoData;

    data += 4;

    box4stco.entry_count = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;

    for (i=0; i<box4stco.entry_count; i++)
    {
        if (i == MAX_STCO_ENTRY_NUM)
        {
            break;
        }

        box4stco.entrys[i].chunk_offset = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

        data += 4;
    }

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\entry_count: %d, [chunk_offset]: ", box4stco.entry_count);

    if (box4stco.entry_count>MAX_STCO_ENTRY_NUM)
    {
        box4stco.entry_count = MAX_STCO_ENTRY_NUM;
    }

    for (i=0; i<box4stco.entry_count; i++)
    {
        if (i>0)
        {
            LOGD(MP4TAG, ", ");
        }

        LOGD(MP4TAG, "[%d]", box4stco.entrys[i].chunk_offset);
    }

    if (box4stco.entry_count==MAX_STCO_ENTRY_NUM)
    {
        LOGD(MP4TAG, "...(just show %d now)", MAX_STCO_ENTRY_NUM);
    }

    LOGD(MP4TAG, "\n");
#endif
}

void DealBox4stbl(const T_BOX *box)
{
    int boxSize = 0;
    int stblDataSize = 0;

    unsigned char *stblData = NULL;
    unsigned char *data = NULL;

    char boxType[MAX_BOX_TYPE_LEN+1] = {0};

    stblData = box->boxData;
    stblDataSize = box->boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN;
    while (stblDataSize > 0)
    {
        boxSize = stblData[0] << 24 | stblData[1] << 16 | stblData[2] << 8 | stblData[3];

        memcpy(boxType, stblData+MAX_BOX_SIZE_LEN, 4);

#ifdef PRINTF_MP4_DEBUG
        LOGD(MP4TAG, "\t\t\t\t\t****BOX: Layer6****\n");
        LOGD(MP4TAG, "\t\t\t\t\t\tsize: %d\n", boxSize);
        LOGD(MP4TAG, "\t\t\t\t\t\ttype: %s\n", boxType);
#endif

        if (0 == strcmp(boxType, BOX_TYPE_STTS))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, stblData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4stts(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_STSS))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, stblData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4stss(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_STSC))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, stblData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4stsc(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_STSZ))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, stblData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4stsz(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_STCO))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, stblData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4stco(data);

                free(data);
                data = NULL;
            }
        }

        stblData += boxSize;
        stblDataSize -= boxSize;
    }
}

void DealBox4mdhd(const unsigned char *mdhdData)
{
    unsigned char *data = NULL;

    T_BOX4MDHD box4mdhd = {0};

    memset(&box4mdhd, 0x0, sizeof(box4mdhd));

    data = (unsigned char *)mdhdData;

    data += 4;
    box4mdhd.creation_time = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mdhd.modification_time = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mdhd.timescale = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mdhd.duration = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

    data += 4;
    box4mdhd.language = data[0] << 8 | data[1];

#ifdef PRINTF_MP4_DEBUG
    //LOGD(MP4TAG, "\t\t\tcreation_time: %d, modification_time: %d, timescale: %d, duration: %d, language: %c%c%c\n",
    //box4mdhd.creation_time, box4mdhd.modification_time, box4mdhd.timescale, box4mdhd.duration, (box4mdhd.language>>10&0x1f), (box4mdhd.language>>5&0x1f), (box4mdhd.language&0x1f));

    LOGD(MP4TAG, "\t\t\t\tcreation_time: %d, modification_time: %d, timescale: %d, duration: %d, language:%d\n",
           box4mdhd.creation_time, box4mdhd.modification_time, box4mdhd.timescale, box4mdhd.duration, box4mdhd.language);
#endif
}

void DealBox4hdlr(const unsigned char *hdlrData)
{
    int i = 0;

    unsigned char *data = NULL;

    T_BOX4HDLR box4hdlr = {0};

    memset(&box4hdlr, 0x0, sizeof(box4hdlr));

    data = (unsigned char *)hdlrData;

    data += 4;
    data += 4;

    memcpy(box4hdlr.handler_type, data, 4);

    box4hdlr.handler_type[MAX_HANDLER_TYPE_LEN] = '\0';

    data += 4;

    data += 12;

    while ('\0' != data[i])
    {
        i++;
    }

    memcpy(box4hdlr.name, data, i);

    box4hdlr.name[MAX_HDLR_NAME_LEN] = '\0';

#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "\t\t\t\thandler_type: %s, name: %s\n", box4hdlr.handler_type, box4hdlr.name);
#endif
}

void DealBox4vmdhd(const unsigned char *vmdhdData)
{
    // TODO
}

void DealBox4minf(const T_BOX *box)
{    int boxSize = 0;
    int minfDataSize = 0;

    unsigned char *minfData = NULL;
    unsigned char *data = NULL;

    char boxType[MAX_BOX_TYPE_LEN+1] = {0};

    T_BOX dinfBox = {0};
    T_BOX stblBox = {0};

    minfData = box->boxData;
    minfDataSize = box->boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN;

    while (minfDataSize > 0)
    {
        boxSize = minfData[0] << 24 | minfData[1] << 16 | minfData[2] << 8 | minfData[3];

        memcpy(boxType, minfData+MAX_BOX_SIZE_LEN, 4);

#ifdef PRINTF_MP4_DEBUG
        LOGD(MP4TAG, "\t\t\t\t********BOX: Layer5********\n");
        LOGD(MP4TAG, "\t\t\t\t\tsize: %d\n", boxSize);
        LOGD(MP4TAG, "\t\t\t\t\ttype: %s\n", boxType);
#endif
        if (0 == strcmp(boxType, BOX_TYPE_VMHD))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, minfData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4vmdhd(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_DINF))
        {
            memset(&dinfBox, 0x0, sizeof(T_BOX));

            dinfBox.boxHeader.boxSize = boxSize;

            memcpy(dinfBox.boxHeader.boxType, boxType, strlen(boxType));

            dinfBox.boxData = (unsigned char*)malloc(boxSize);
            if (dinfBox.boxData)
            {
                memcpy(dinfBox.boxData, minfData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4dinf((const T_BOX*)&dinfBox);

                free(dinfBox.boxData);
                dinfBox.boxData = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_STBL))
        {
            memset(&stblBox, 0x0, sizeof(T_BOX));

            stblBox.boxHeader.boxSize = boxSize;

            memcpy(stblBox.boxHeader.boxType, boxType, strlen(boxType));

            stblBox.boxData = (unsigned char*)malloc(boxSize);
            if (stblBox.boxData)
            {
                memcpy(stblBox.boxData, minfData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4stbl((const T_BOX*)&stblBox);

                free(stblBox.boxData);
                stblBox.boxData = NULL;
            }
        }

        minfData += boxSize;
        minfDataSize -= boxSize;
    }
}

void DealBox4mdia(const T_BOX *box)
{    int boxSize = 0;
    int mdiaDataSize = 0;

    unsigned char *mdiaData = NULL;
    unsigned char *data = NULL;

    char boxType[MAX_BOX_TYPE_LEN+1] = {0};

    T_BOX minfBox = {0};

    mdiaData = box->boxData;
    mdiaDataSize = box->boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN;

    while (mdiaDataSize > 0)
    {
        boxSize = mdiaData[0] << 24 | mdiaData[1] << 16 | mdiaData[2] << 8 | mdiaData[3];

        memcpy(boxType, mdiaData+MAX_BOX_SIZE_LEN, 4);

#ifdef PRINTF_MP4_DEBUG
        LOGD(MP4TAG, "\t\t\t************BOX: Layer4************\n");
        LOGD(MP4TAG, "\t\t\t\tsize: %d\n", boxSize);
        LOGD(MP4TAG, "\t\t\t\ttype: %s\n", boxType);
#endif
        if (0 == strcmp(boxType, BOX_TYPE_MDHD))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, mdiaData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4mdhd(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_HDLR))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, mdiaData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4hdlr(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_MINF))
        {
            memset(&minfBox, 0x0, sizeof(T_BOX));

            minfBox.boxHeader.boxSize = boxSize;

            memcpy(minfBox.boxHeader.boxType, boxType, strlen(boxType));

            minfBox.boxData = (unsigned char*)malloc(boxSize);
            if (minfBox.boxData)
            {
                memcpy(minfBox.boxData, mdiaData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4minf((const T_BOX*)&minfBox);

                free(minfBox.boxData);
                minfBox.boxData = NULL;
            }
        }

        mdiaData += boxSize;
        mdiaDataSize -= boxSize;
    }
}

void DealBox4trak(const T_BOX *box)
{
    int boxSize = 0;
    int trakDataSize = 0;

    unsigned char *trakData = NULL;
    unsigned char *data = NULL;

    char boxType[MAX_BOX_TYPE_LEN+1] = {0};

    T_BOX mdiaBox = {0};

    trakData = box->boxData;
    trakDataSize = box->boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN;

    while (trakDataSize > 0)
    {
        boxSize = trakData[0] << 24 | trakData[1] << 16 | trakData[2] << 8 | trakData[3];

        memcpy(boxType, trakData+MAX_BOX_SIZE_LEN, 4);

#ifdef PRINTF_MP4_DEBUG
        LOGD(MP4TAG, "\t\t****************BOX: Layer3****************\n");
        LOGD(MP4TAG, "\t\t\tsize: %d\n", boxSize);
        LOGD(MP4TAG, "\t\t\ttype: %s\n", boxType);
#endif

        if (0 == strcmp(boxType, BOX_TYPE_TKHD))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, trakData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4tkhd(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_MDIA))
        {
            memset(&mdiaBox, 0x0, sizeof(T_BOX));

            mdiaBox.boxHeader.boxSize = boxSize;

            memcpy(mdiaBox.boxHeader.boxType, boxType, strlen(boxType));

            mdiaBox.boxData = (unsigned char*)malloc(boxSize);
            if (mdiaBox.boxData)
            {
                memcpy(mdiaBox.boxData, trakData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4mdia((const T_BOX*)&mdiaBox);

                free(mdiaBox.boxData);
                mdiaBox.boxData = NULL;
            }
        }

        trakData += boxSize;
        trakDataSize -= boxSize;
    }
}

void DealBox4moov(const T_BOX *box)
{
    int boxSize = 0;
    int moovDataSize = 0;

    unsigned char *moovData = NULL;
    unsigned char *data = NULL;

    char boxType[MAX_BOX_TYPE_LEN+1] = {0};

    T_BOX trakBox = {0};

    moovData = box->boxData;
    moovDataSize = box->boxHeader.boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN;

    while (moovDataSize > 0)
    {
        boxSize = moovData[0] << 24 | moovData[1] << 16 | moovData[2] << 8 | moovData[3];

        memcpy(boxType, moovData+MAX_BOX_SIZE_LEN, 4);

        boxType[MAX_BOX_TYPE_LEN] = '\0';

#ifdef PRINTF_MP4_DEBUG
        LOGD(MP4TAG, "\t********************BOX: Layer2********************\n");
        LOGD(MP4TAG, "\t\tsize: %d\n", boxSize);
        LOGD(MP4TAG, "\t\ttype: %s\n", boxType);
#endif

        if (0 == strcmp(boxType, BOX_TYPE_MVHD))
        {
            data = (unsigned char*)malloc(boxSize);
            if (data)
            {
                memcpy(data, moovData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4mvhd(data);

                free(data);
                data = NULL;
            }
        }
        else if (0 == strcmp(boxType, BOX_TYPE_TRAK))
        {
            memset(&trakBox, 0x0, sizeof(T_BOX));

            trakBox.boxHeader.boxSize = boxSize;

            memcpy(trakBox.boxHeader.boxType, boxType, strlen(boxType));

            trakBox.boxData = (unsigned char*)malloc(boxSize);
            if (trakBox.boxData)
            {
                memcpy(trakBox.boxData, moovData+MAX_BOX_SIZE_LEN+MAX_BOX_TYPE_LEN, boxSize-MAX_BOX_SIZE_LEN-MAX_BOX_TYPE_LEN);

                DealBox4trak((const T_BOX*)&trakBox);

                free(trakBox.boxData);
                trakBox.boxData = NULL;
            }
        }

        moovData += boxSize;
        moovDataSize -= boxSize;
    }
}

void DealBox(const T_BOX *box)
{
#ifdef PRINTF_MP4_DEBUG
    LOGD(MP4TAG, "****************************BOX: Layer1****************************\n");
    LOGD(MP4TAG, "\tsize: %d\n", box->boxHeader.boxSize);
    LOGD(MP4TAG, "\ttype: %s\n", box->boxHeader.boxType);
#endif

    if (0 == strcmp(reinterpret_cast<const char *>(box->boxHeader.boxType), BOX_TYPE_FTYPE))
    {
        DealBox4ftyp(box);
    }
    else if (0 == strcmp(reinterpret_cast<const char *>(box->boxHeader.boxType), BOX_TYPE_MOOV))
    {
        DealBox4moov(box);
    }
}