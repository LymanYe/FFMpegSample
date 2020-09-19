//
// Created by Lyman on 2020-09-19.
//

#include "DeMuxing.h"



DeMuxing::DeMuxing(char *inputfilepath, char *outputDir) {
    inputfilename = inputfilepath;
    char input_vchars[150];
    sprintf(input_vchars, "%s/%s", outputDir, "output.h264");
    outputfilename_v = input_vchars;
    char input_achars[150];
    sprintf(input_achars, "%s/%s", outputDir, "output.mp3");
    outputfilename_a = input_achars;

    //Input
    formatContext = avformat_alloc_context();
    if ((ret = avformat_open_input(&formatContext, inputfilename, 0, 0)) < 0) {
        LOGE(DEMUXING_TAG, "Could not open input file.");
        return;
    }
    if ((ret = avformat_find_stream_info(formatContext, 0)) < 0) {
        LOGE(DEMUXING_TAG, "Failed to retrieve input stream information");
        return;
    }

    videoindex = -1;
    for(i = 0; i < formatContext->nb_streams; i++) {
        if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex = i;
        }else if(formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioindex = i;
        }
    }

    //Dump Format------------------
    LOGE(DEMUXING_TAG, "\nInput Video===========================\n");
    av_dump_format(formatContext, 0, inputfilename, 0);
    LOGE(DEMUXING_TAG, "\n======================================\n");

    fp_audio = fopen(outputfilename_a, "wb+");
    fp_video = fopen(outputfilename_v, "wb+");
}


DeMuxing::~DeMuxing() {

}


void DeMuxing::demuxingSampleVideo() {
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
    avcodec_parameters_copy(ctx->par_in, formatContext->streams[videoindex]->codecpar);
    av_bsf_init(ctx);
#endif
    while(av_read_frame(formatContext, &pkt) >= 0){
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

    avformat_close_input(&formatContext);
}
