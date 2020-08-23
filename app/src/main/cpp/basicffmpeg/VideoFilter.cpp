//
// Created by Lyman on 2020-08-22.
//

#include "VideoFilter.h"

void VideoFilter::initSingleYUVFile(char *inputfilepath, char *outputfilepath, int width, int height) {
    //Input YUV
    fp_in = fopen(inputfilepath,"rb+");
    if(fp_in == NULL){
        LOGE(VIDEO_FILTER_TAG, "initSingleYUVFile, Error open input file.\n");
        return;
    }
    in_width = width;
    in_height = height;

    //Output YUV
    fp_out = fopen(outputfilepath,"wb+");
    if(fp_out == NULL){
        LOGE(VIDEO_FILTER_TAG, "initSingleYUVFile, Error open output file.\n");
        return;
    }
}


void VideoFilter::initFilter() {
//const char *filter_descr = "lutyuv='u=128:v=128'";
    //const char *filter_descr = "boxblur";
    //const char *filter_descr = "hflip";
    const char *filter_descr = "hue='h=60:s=-3'";
    //const char *filter_descr = "crop=2/3*in_w:2/3*in_h";
    //const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=pink@0.5";
    //const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=green:fontsize=30:text='Lyman'";

    char args[512];
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    //const enum PixelFormat* pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    const enum AVPixelFormat *pix_fmts = (const AVPixelFormat *) AV_PIX_FMT_YUV420P;
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             in_width,in_height,AV_PIX_FMT_YUV420P,
             1, 25,1,1);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE(VIDEO_FILTER_TAG, "initFilter, Cannot create buffer source, ret: %d \n", ret);
        return;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOGE(VIDEO_FILTER_TAG, "initFilter,Cannot create buffer sink, error: %s \n", av_err2str(ret));
        return;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                        &inputs, &outputs, NULL)) < 0)
        return;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        return;

    frame_in=av_frame_alloc();
    frame_buffer_in=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width,in_height,1));
    av_image_fill_arrays(frame_in->data, frame_in->linesize,frame_buffer_in,
                         AV_PIX_FMT_YUV420P,in_width, in_height,1);

    frame_out=av_frame_alloc();
    frame_buffer_out=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width,in_height,1));
    av_image_fill_arrays(frame_out->data, frame_out->linesize,frame_buffer_out,
                         AV_PIX_FMT_YUV420P,in_width, in_height,1);

    frame_in->width=in_width;
    frame_in->height=in_height;
    frame_in->format=AV_PIX_FMT_YUV420P;
}


void VideoFilter::filterFrame() {
    while (1) {
        if(fread(frame_buffer_in, 1, in_width * in_height * 3 / 2, fp_in) != in_width * in_height * 3 / 2){
            break;
        }
        //input Y,U,V
        frame_in->data[0]=frame_buffer_in;
        frame_in->data[1]=frame_buffer_in+in_width*in_height;
        frame_in->data[2]=frame_buffer_in+in_width*in_height*5/4;

        if (av_buffersrc_add_frame(buffersrc_ctx, frame_in) < 0) {
            LOGE(VIDEO_FILTER_TAG, "filterFrame, Error while add frame.\n");
            break;
        }

        /* pull filtered pictures from the filtergraph */
        ret = av_buffersink_get_frame(buffersink_ctx, frame_out);
        if (ret < 0)
            break;

        //output Y,U,V
        if(frame_out->format == AV_PIX_FMT_YUV420P){
            for(int i=0;i<frame_out->height;i++){
                fwrite(frame_out->data[0]+frame_out->linesize[0]*i,1,frame_out->width,fp_out);
            }
            for(int i=0;i<frame_out->height/2;i++){
                fwrite(frame_out->data[1]+frame_out->linesize[1]*i,1,frame_out->width/2,fp_out);
            }
            for(int i=0;i<frame_out->height/2;i++){
                fwrite(frame_out->data[2]+frame_out->linesize[2]*i,1,frame_out->width/2,fp_out);
            }
        }
        LOGE(VIDEO_FILTER_TAG, "filterFrame, Process 1 frame!\n");
        av_frame_unref(frame_out);
    }

    fclose(fp_in);
    fclose(fp_out);

    av_frame_free(&frame_in);
    av_frame_free(&frame_out);
    avfilter_graph_free(&filter_graph);
}