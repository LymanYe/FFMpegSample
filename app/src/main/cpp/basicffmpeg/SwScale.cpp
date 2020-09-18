//
// Created by Lyman on 2020-09-18.
//

#include "SwScale.h"

SwScale::SwScale(char *outputfilepath) {
    dst_filename = outputfilepath;
    char dst_size_str[50];
    sprintf(dst_size_str, "%dx%d", src_w, src_h);
    dst_size = dst_size_str;

    if (av_parse_video_size(&dst_w, &dst_h, dst_size) < 0) {
        LOGE(SWSCALE_TAG,
                "Invalid size '%s', must be in the form WxH or a valid size abbreviation\n",
                dst_size);
        return ;
    }

    LOGD(SWSCALE_TAG, "SwScale, dst_w = %d, dst_h = %d, dst_size = %s", dst_w, dst_h, dst_size);

    dst_file = fopen(dst_filename, "wb");
    if (!dst_file) {
        LOGE(SWSCALE_TAG, "Could not open destination file %s\n", dst_filename);
        return ;
    }

    /* create scaling context */
    sws_ctx = sws_getContext(src_w, src_h, src_pix_fmt,
                             dst_w, dst_h, dst_pix_fmt,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        LOGE(SWSCALE_TAG,
                "Impossible to create scale context for the conversion "
                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                av_get_pix_fmt_name(src_pix_fmt), src_w, src_h,
                av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h);
        ret = AVERROR(EINVAL);
        return ;
    }

    /* allocate source and destination image buffers */
    if ((ret = av_image_alloc(src_data, src_linesize,
                              src_w, src_h, src_pix_fmt, 16)) < 0) {
        LOGE(SWSCALE_TAG, "Could not allocate source image\n");
        return ;
    }

    /* buffer is going to be written to rawvideo file, no alignment */
    if ((ret = av_image_alloc(dst_data, dst_linesize,
                              dst_w, dst_h, dst_pix_fmt, 1)) < 0) {
        LOGE(SWSCALE_TAG, "Could not allocate destination image\n");
        return ;
    }
    dst_bufsize = ret;

}


SwScale::~SwScale() {

}


void fill_yuv_image(uint8_t *data[4], int linesize[4],
                           int width, int height, int frame_index)
{
    int x, y;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            data[0][y * linesize[0] + x] = x + y + frame_index * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            data[1][y * linesize[1] + x] = 128 + y + frame_index * 2;
            data[2][y * linesize[2] + x] = 64 + x + frame_index * 5;
        }
    }
}


void SwScale::ConvertYUV420P2RGB() {
    for (i = 0; i < 100; i++) {
        /* generate synthetic video */
        fill_yuv_image(src_data, src_linesize, src_w, src_h, i);

        /* convert to destination format */
        sws_scale(sws_ctx, (const uint8_t * const*)src_data,
                  src_linesize, 0, src_h, dst_data, dst_linesize);

        /* write scaled image to file */
        fwrite(dst_data[0], 1, dst_bufsize, dst_file);
    }

    LOGD(SWSCALE_TAG, "ConvertYUV420P2RGB, Scaling succeeded. Play the output file with the command:\n"
                    "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
            av_get_pix_fmt_name(dst_pix_fmt), dst_w, dst_h, dst_filename);


    fclose(dst_file);
    av_freep(&src_data[0]);
    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);
}