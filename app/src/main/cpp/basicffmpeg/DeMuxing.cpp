//
// Created by Lyman on 2020-09-19.
//

#include "DeMuxing.h"



DeMuxing::DeMuxing() {

}


DeMuxing::~DeMuxing() {

}


void DeMuxing::initDemuxingSample(char *inputfilepath, char *outputDir) {
    inputfilename = inputfilepath;
    char input_vchars[150];
    sprintf(input_vchars, "%s/%s", outputDir, "output.h264");
    outputfilename_v = input_vchars;
    char input_achars[150];
    sprintf(input_achars, "%s/%s", outputDir, "output.mp3");
    outputfilename_a = input_achars;

    //Input
    inputFormatContext = avformat_alloc_context();
    if ((ret = avformat_open_input(&inputFormatContext, inputfilename, 0, 0)) < 0) {
        LOGE(DEMUXING_TAG, "Could not open input file.");
        return;
    }
    if ((ret = avformat_find_stream_info(inputFormatContext, 0)) < 0) {
        LOGE(DEMUXING_TAG, "Failed to retrieve input stream information");
        return;
    }

    for(i = 0; i < inputFormatContext->nb_streams; i++) {
        if(inputFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
        }else if(inputFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioindex = i;
        }
    }

    //Dump Format------------------
    LOGE(DEMUXING_TAG, "\nInput Video===========================\n");
    av_dump_format(inputFormatContext, 0, inputfilename, 0);
    LOGE(DEMUXING_TAG, "\n======================================\n");

    fp_audio = fopen(outputfilename_a, "wb+");
    fp_video = fopen(outputfilename_v, "wb+");
}


//demuxing h264 & mp3
void DeMuxing::demuxingSample() {
    const AVBitStreamFilter *h264bsfc;
    AVBSFContext *ctx;
    /*
    FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
    "h264_mp4toannexb" bitstream filter (BSF)
      *Add SPS,PPS in front of IDR frame
      *Add start code ("0,0,0,1") in front of NALU
    H.264 in some container (MPEG2TS) don't need this BSF.
    */
#if USE_H264BSF
    h264bsfc = av_bsf_get_by_name("h264_mp4toannexb");
    ret = av_bsf_alloc(h264bsfc, &ctx);
    if (ret < 0)
        return;
    avcodec_parameters_copy(ctx->par_in, inputFormatContext->streams[videoindex]->codecpar);
    av_bsf_init(ctx);
#endif
    while(av_read_frame(inputFormatContext, &pkt) >= 0){
        if(pkt.stream_index == videoindex){
#if USE_H264BSF
            ret = av_bsf_send_packet(ctx, &pkt);
            if(ret < 0) {
                LOGE(DEMUXING_TAG, "av_bsf_send_packet failed: %s", av_err2str(ret));
                break;
            }
            while ((ret = av_bsf_receive_packet(ctx, &pkt)) == 0) {
                LOGD(DEMUXING_TAG, "Write Video Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
                fwrite(pkt.data, pkt.size, 1, fp_video);
            }
#else

            LOGD(DEMUXING_TAG, "Write Video Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
            fwrite(pkt.data, 1, pkt.size, fp_video);
#endif
        }else if(pkt.stream_index == audioindex){
            /*
            AAC in some container format (FLV, MP4, MKV etc.) need to add 7 Bytes
            ADTS Header in front of AVPacket data manually.
            Other Audio Codec (MP3...) works well.
            */
            LOGD(DEMUXING_TAG, "Write Audio Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
            fwrite(pkt.data, 1, pkt.size, fp_audio);
        }
        av_packet_unref(&pkt);
    }

#if USE_H264BSF
    av_bsf_free(&ctx);
#endif


    fclose(fp_video);
    fclose(fp_audio);

    avformat_close_input(&inputFormatContext);
}


void DeMuxing::initDemuxing(char *inputfilepath, char *outputDir) {
    inputfilename = inputfilepath;
    char input_vchars[150];
    sprintf(input_vchars, "%s/%s", outputDir, "output.h264");
    outputfilename_v = input_vchars;
    char input_achars[150];
    sprintf(input_achars, "%s/%s", outputDir, "output.aac");
    outputfilename_a = input_achars;

    //Input
    inputFormatContext = avformat_alloc_context();
    if ((ret = avformat_open_input(&inputFormatContext, inputfilename, 0, 0)) < 0) {
        LOGE(DEMUXING_TAG, "initDemuxing, Could not open input file.");
        return;
    }
    if ((ret = avformat_find_stream_info(inputFormatContext, 0)) < 0) {
        LOGE(DEMUXING_TAG, "initDemuxing, Failed to retrieve input stream information");
        return;
    }

    //Output
    avformat_alloc_output_context2(&outputFormatContextV, NULL, NULL, outputfilename_v);
    if(!outputFormatContextV) {
        LOGE(DEMUXING_TAG, "initDemuxing, Could not create video output context");
        return;
    }
    outputFormatV = outputFormatContextV->oformat;
    avformat_alloc_output_context2(&outputFormatContextA, NULL, NULL, outputfilename_a);
    if(!outputFormatContextA) {
        LOGE(DEMUXING_TAG, "initDemuxing, Could not create audio output context");
        return;
    }
    outputFormatA = outputFormatContextA->oformat;


    for(i = 0; i < inputFormatContext->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVFormatContext *ofmt_ctx;
        AVStream *in_stream = inputFormatContext->streams[i];
        AVStream *out_stream = NULL;
        AVCodecContext * codec_ctx;
        AVCodec *codec;


        if(inputFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
            out_stream = avformat_new_stream(outputFormatContextV, NULL);
            ofmt_ctx = outputFormatContextV;
            codec = avcodec_find_decoder(in_stream->codecpar->codec_id);
            LOGD(DEMUXING_TAG, "initDemuxing, find video stream");
        }else if(inputFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioindex = i;
            out_stream = avformat_new_stream(outputFormatContextA, NULL);
            ofmt_ctx = outputFormatContextA;
            codec = avcodec_find_decoder(in_stream->codecpar->codec_id);
            LOGD(DEMUXING_TAG, "initDemuxing, find audio stream");
        }

        if (!out_stream) {
            LOGE(DEMUXING_TAG, "initDemuxing, Failed allocating output stream\n");
            return;
        }
        //Copy the settings of AVCodecContext
        codec_ctx = avcodec_alloc_context3(codec);
        ret = avcodec_parameters_to_context(codec_ctx, in_stream->codecpar);
        if (ret < 0){
            LOGE(DEMUXING_TAG, "initDemuxing, Failed to copy in_stream codecpar to codec context\n");
            return;
        }

        codec_ctx->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
        if (ret < 0){
            LOGE(DEMUXING_TAG, "initDemuxing, Failed to copy codec context to out_stream codecpar context\n");
            return;
        }
    }

    //Dump Format------------------
    LOGD(DEMUXING_TAG, "initDemuxing, \n==============Input Video=============\n");
    av_dump_format(inputFormatContext, 0, inputfilepath, 0);
    LOGD(DEMUXING_TAG, "initDemuxing, \n==============Output Video============\n");
    av_dump_format(outputFormatContextV, 0, outputfilename_v, 1);
    LOGD(DEMUXING_TAG, "initDemuxing, \n==============Output Audio============\n");
    av_dump_format(outputFormatContextA, 0, outputfilename_a, 1);
    LOGD(DEMUXING_TAG, "initDemuxing, \n======================================\n");
    //Open output file
    if (!(outputFormatV->flags & AVFMT_NOFILE)) {
        if (avio_open(&outputFormatContextV->pb, outputfilename_v, AVIO_FLAG_WRITE) < 0) {
            LOGE(DEMUXING_TAG, "initDemuxing, Could not open output file '%s'", outputfilename_v);
            return;
        }
    }

    if (!(outputFormatA->flags & AVFMT_NOFILE)) {
        if (avio_open(&outputFormatContextA->pb, outputfilename_a, AVIO_FLAG_WRITE) < 0) {
            printf( "initDemuxing, Could not open output file '%s'", outputfilename_a);
            return;
        }
    }
}

// demuxing h264 & aac data
void DeMuxing::demuxing() {
    //Write file header
    ret = avformat_write_header(outputFormatContextA, NULL);
    if (ret < 0) {
        LOGE(DEMUXING_TAG, "demuxing, Error occurred when opening audio output file: %s\n", av_err2str(ret));
        return;
    }
    ret = avformat_write_header(outputFormatContextV, NULL);
    if (ret < 0) {
        LOGE(DEMUXING_TAG, "demuxing, Error occurred when opening video output file: %s\n", av_err2str(ret));
        return;
    }

    const AVBitStreamFilter *h264bsfc;
    AVBSFContext *ctx;
    /*
    FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
    "h264_mp4toannexb" bitstream filter (BSF)
      *Add SPS,PPS in front of IDR frame
      *Add start code ("0,0,0,1") in front of NALU
    H.264 in some container (MPEG2TS) don't need this BSF.
    */
#if USE_H264BSF
    h264bsfc = av_bsf_get_by_name("h264_mp4toannexb");
    ret = av_bsf_alloc(h264bsfc, &ctx);
    if (ret < 0)
        return;
    avcodec_parameters_copy(ctx->par_in, inputFormatContext->streams[videoindex]->codecpar);
    av_bsf_init(ctx);
#endif
    while(av_read_frame(inputFormatContext, &pkt) >= 0){
        AVFormatContext *ofmt_ctx;
        AVStream *in_stream, *out_stream;
        in_stream = inputFormatContext->streams[pkt.stream_index];

        if(pkt.stream_index == videoindex){
            out_stream = outputFormatContextV->streams[0];
            ofmt_ctx = outputFormatContextV;
#if USE_H264BSF
            ret = av_bsf_send_packet(ctx, &pkt);
            if(ret < 0) {
                LOGE(DEMUXING_TAG, "demuxing, av_bsf_send_packet failed: %s", av_err2str(ret));
                break;
            }
            while ((ret = av_bsf_receive_packet(ctx, &pkt)) == 0) {
                LOGD(DEMUXING_TAG, "demuxing, Write Video Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
                //Convert PTS/DTS
                pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
                pkt.pos = -1;
                pkt.stream_index=0;
                //Write
                if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
                    LOGE(DEMUXING_TAG, "demuxing, Error muxing packet\n");
                    break;
                }
            }
#else

            LOGD(DEMUXING_TAG, "Write Video Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
            //Convert PTS/DTS
                pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
                pkt.pos = -1;
                pkt.stream_index=0;
                //Write
                if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
                    LOGE(DEMUXING_TAG, "demuxing, Error muxing packet\n");
                    break;
                }
#endif
        }else if(pkt.stream_index == audioindex){
            out_stream = outputFormatContextA->streams[0];
            ofmt_ctx = outputFormatContextA;
            LOGD(DEMUXING_TAG, "Write Audio Packet. size:%d\tpts:%lld\n", pkt.size, pkt.pts);
            //Convert PTS/DTS
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
            pkt.stream_index=0;
            //Write
            if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
                LOGE(DEMUXING_TAG, "demuxing, Error muxing packet\n");
                break;
            }
        }
        av_packet_unref(&pkt);
    }

#if USE_H264BSF
    av_bsf_free(&ctx);
#endif

    //Write file trailer
    av_write_trailer(outputFormatContextA);
    av_write_trailer(outputFormatContextV);

    // release
    avformat_close_input(&inputFormatContext);
    /* close output */
    if (outputFormatContextA && !(outputFormatA->flags & AVFMT_NOFILE))
        avio_close(outputFormatContextA->pb);

    if (outputFormatContextV && !(outputFormatV->flags & AVFMT_NOFILE))
        avio_close(outputFormatContextV->pb);

    avformat_free_context(outputFormatContextA);
    avformat_free_context(outputFormatContextV);
}
