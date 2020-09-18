//
// Created by Lyman on 2020/8/20.
//


#include "VideoEncoder.h"

VideoEncoder::VideoEncoder(char *inputfilepath, char *outputfilepath, int width, int height, AVCodecID codecId) {
    filename = inputfilepath;
//    codec = avcodec_find_encoder_by_name(codec_name);
    codec = avcodec_find_encoder(codecId);
    if (!codec) {
        LOGE(VIDEO_ENCODE_TAG, "Codec '%d' not found\n", codecId);
        return ;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate video codec context\n");
        return ;
    }

    pkt = av_packet_alloc();
    if (!pkt)
        return ;

    this->width = width;
    this->height = height;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = height;
    /* frames per second */
    c->time_base = (AVRational){1, 25};
    c->framerate = (AVRational){25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264) {
        av_opt_set(c->priv_data, "preset", "slow", 0);
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
    }

    if (codec->id == AV_CODEC_ID_H265) {
        av_opt_set(c->priv_data, "preset", "ultrafast", 0);
        av_opt_set(c->priv_data, "tune", "zero-latency", 0);
    }


    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Could not open codec: %s\n", av_err2str(ret));
        return ;
    }

    if(inputfilepath != NULL) {
        inputFile = fopen(inputfilepath, "rb");
        if (!inputFile) {
            LOGE(VIDEO_ENCODE_TAG, "Could not open %s\n", inputfilepath);
            return ;
        }
    }

    if(outputfilepath != NULL) {
        outputFile = fopen(outputfilepath, "wb");
        if (!outputFile) {
            LOGE(VIDEO_ENCODE_TAG, "Could not open %s\n", outputfilepath);
            return ;
        }
    }


    frame = av_frame_alloc();
    if (!frame) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate video frame\n");
        return ;
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate the video frame data\n");
        return ;
    }



    // 使用avformat协助处理stream的相关信息

    //为输出格式初始化AVFormatContext指针。
    avformat_alloc_output_context2(&avFormatContext, NULL, NULL, outputfilepath);
    //创建一个用于输出的AVStream指针对象
    avStream = avformat_new_stream(avFormatContext, 0);
    // 打开上下文用于准备写入数据
    ret = avio_open(&avFormatContext->pb, outputfilepath, AVIO_FLAG_READ_WRITE);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "VideoEncoder, avio_open fail %d",ret);
        return;
    }
    /** 写入头文件信息   重要：将编码器信息拷贝到AVFormatContext的对应的AVSream流中
     *  解决方案：通过avcodec_parameters_from_context方法将编码器信息拷贝到输出流的AVStream中codecpar参数
    */
    avcodec_parameters_from_context(avStream->codecpar, c);
    av_dump_format(avFormatContext, 0, outputfilepath, 1);
}


VideoEncoder::VideoEncoder(char *outputfilepath, AVCodecID codecId) {
//    codec = avcodec_find_encoder_by_name(codec_name);
    codec = avcodec_find_encoder(codecId);
    if (!codec) {
        LOGE(VIDEO_ENCODE_TAG, "Codec '%d' not found\n", codecId);
        return ;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate video codec context\n");
        return ;
    }

    pkt = av_packet_alloc();
    if (!pkt)
        return ;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 352;
    c->height = 288;
    /* frames per second */
    c->time_base = (AVRational){1, 25};
    c->framerate = (AVRational){25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Could not open codec: %s\n", av_err2str(ret));
        return ;
    }

    if(outputfilepath != NULL) {
        outputFile = fopen(outputfilepath, "wb");
        if (!outputFile) {
            LOGE(VIDEO_ENCODE_TAG, "Could not open %s\n", outputfilepath);
            return ;
        }
    }


    frame = av_frame_alloc();
    if (!frame) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate video frame\n");
        return ;
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Could not allocate the video frame data\n");
        return ;
    }
}


void VideoEncoder::encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        LOGD(VIDEO_ENCODE_TAG, "Send frame %3" PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(VIDEO_ENCODE_TAG, "Error during encoding\n");
            return;
        }

        LOGD(VIDEO_ENCODE_TAG, "Write packet %3" PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}


void VideoEncoder::encodeH264OrH265(AVFormatContext *fmtContext, AVCodecContext *cCtx, AVFrame *frame, AVPacket *pkt,
                          FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        LOGD(VIDEO_ENCODE_TAG, "Send frame %3" PRId64"\n", frame->pts);

    ret = avcodec_send_frame(cCtx, frame);
    if (ret < 0) {
        LOGE(VIDEO_ENCODE_TAG, "Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(cCtx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGE(VIDEO_ENCODE_TAG, "Error during encoding\n");
            return;
        }

        LOGD(VIDEO_ENCODE_TAG, "Write packet %3" PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        av_write_frame(fmtContext, pkt);
        if (outfile) {
            fwrite(pkt->data, pkt->size, 1, outfile);
        }
        av_packet_unref(pkt);
    }
}


void VideoEncoder::encodeGenerateYUV420P2H264() {
/* encode 1 second of video */
    for (i = 0; i < 25; i++) {
        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            LOGE(VIDEO_ENCODE_TAG, "encodeGenerateYUV420P2H264, make frame writable ret = %d", ret);
            break;
        }

        /* prepare a dummy image */
        /* Y */
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for (y = 0; y < c->height/2; y++) {
            for (x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, outputFile);
    }

    /* flush the encoder */
    encode(c, NULL, pkt, outputFile);

    /* add sequence end code to have a real MPEG file */
//    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
//    fwrite(endcode, 1, sizeof(endcode), outputFile);
    fclose(outputFile);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);
}


/* YUV格式有两大类：planar和packed。
planar：YUV的存储中与RGB格式最大不同在于，RGB格式每个点的数据是连继保存在一起的。
即R，G，B是前后不间隔的保存在2-4byte空间中。而YUV的数据中为了节约空间，U，V分量空间会减小。
每一个点的Y分量独立保存，但连续几个点的U，V分量是保存在一起的.这几个点合起来称为macro-pixel，
这种存储格式称为Packed(打包)格式。对于planar的YUV格式，先连续存储所有像素点的Y，
紧接着存储所有像素点的U，随后是所有像素点的V。
八个像素为：[Y0 U0 V0] [Y1 U1 V1] [Y2 U2 V2] [Y3 U3 V3]
[Y5 U5 V5] [Y6 U6 V6] [Y7U7 V7] [Y8 U8 V8]
存放的码流为：Y0 U0 Y1 Y2 U2 Y3
Y5 V5 Y6 Y7 V7 Y8
映射出的像素点为：[Y0 U0 V5] [Y1 U0 V5] [Y2 U2 V7] [Y3 U2 V7]
[Y5 U0 V5] [Y6 U0 V5] [Y7U2 V7] [Y8 U2 V7]
内存则是：yyyyyyyyuuvv
        需要占用的内存：w * h * 3 / 2
如果视频帧的宽和高分别为w和h，那么一帧YUV420P像素数据一共占用w*h*3/2 Byte的数据。
其中前w*h Byte存储Y，接着的w*h*1/4 Byte存储U，最后w*h*1/4 Byte存储V
*/

void VideoEncoder::encodeYUV420PImage2H264() {
    int picture_size = av_image_get_buffer_size(c->pix_fmt, c->width, c->height, 1);
    uint8_t *picture_buf = (uint8_t *) av_malloc(picture_size);
    /*
     *根据指定的图像参数和提供的数组设置数据指针和行大小。
     * 通过指向图像数据缓冲区的src地址填充给定图像的属性。
     * 根据指定的像素格式，设置一个或多个图像数据指针和行大小。
     * 如果指定了平面格式，那么将设置指向不同图片平面的几个指针，
     * 并且不同平面的行大小将存储在linesize数组中。
     * 调用src==NULL，以获得src缓冲区所需的大小。
     * 要在一个调用中分配缓冲区并填写dst_data和dst_linesize，请使用av_Image_alloc()。
     * @param 将要被填充的dst_data数据指针，
     * @param 将要被填充的dst_datast的行大小
     * @param src 将包含或包含实际图像数据的缓冲区，可以为NULL
     * @param 图像的像素格式
     * @param 图像的宽度，以像素为单位
     * @param 图像的高度，以像素为单位
     * @param src中用于行大小对齐的值
     * @返回 src所需的大小，在发生故障时返回负错误码，
     */
    av_image_fill_arrays(frame->data, frame->linesize, picture_buf, c->pix_fmt,
                         c->width, c->height, 1);
    //初始化流的私有数据并将流头写入输出媒体文件
    avformat_write_header(avFormatContext, NULL);

    int rsize = width * height + width * height / 2;
    uint8_t *rbuf = new uint8_t[rsize];
    /* encode 1 second of video */
    //for (i = 0; i < 25; i++) {
        fflush(stdout);
        memset(rbuf, 0, rsize);
        uint8_t *ptr = rbuf;
        ret = fread(rbuf, 1, rsize, inputFile);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            LOGE(VIDEO_ENCODE_TAG, "encodeYUV420PImage2H264, av_frame_make_writable ret = %d", ret);
            return;
        }

        /* prepare a dummy image */
        memcpy(frame->data[0], rbuf, width * height);
#if 1
        //YUV420P格式
        memcpy(frame->data[1], rbuf + width * height, width * height / 4);
        memcpy(frame->data[2], rbuf + width * height + width * height / 4, width * height / 4);
#else
        //NV12格式
        uint8_t *uptr = rbuf + width*height, *vptr = rbuf + width*height*5/4;
        uint8_t *dst = &pFrame->data[1][0];
        for (int ii = 0; ii < height*width/4; ii++) {
            *dst++ = *uptr++;
            *dst++ = *vptr++;
        }
#endif
        //frame->pts = i;

    /**
     * 举例来说：
    FPS = 30，time_base = {1, 90000}，则
    duration = (1 / 30) / (1 / 90000) = 3000，
    可以这么理解：
    帧率是 30 HZ，每帧的持续时间是 1 / 30 秒，时间单位是 1 / 90000 秒，  那么，每帧的 duration 就是 3000 个时间单位。
     编码器产生的帧，直接存入某个容器的AVStream中，那么此时packet的Time要从AVCodecContext的time转换成目标AVStream的time
     */
        frame->pts = i++ * c->time_base.num / c->time_base.den /
                       (avStream->time_base.num / avStream->time_base.num);

        /* encode the image */
        encodeH264OrH265(avFormatContext, c, frame, pkt, outputFile);
    //}

    /* flush the encoder */
    encodeH264OrH265(avFormatContext, c, NULL, pkt, outputFile);
    av_write_trailer(avFormatContext);

    LOGD(VIDEO_ENCODE_TAG, "encodeYUV420PImage2H264, success");
    fclose(inputFile);
    fclose(outputFile);

    avcodec_free_context(&c);
    avformat_free_context(avFormatContext);
    av_frame_free(&frame);
    av_packet_free(&pkt);

}


VideoEncoder::~VideoEncoder() {

}
