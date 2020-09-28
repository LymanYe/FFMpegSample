//
// Created by Lyman on 2020-09-21.
//


#include "Muxing.h"

Muxing::Muxing() {

}


static void releaseContext(AVFormatContext **in_fmt1,AVFormatContext **in_fmt2,AVFormatContext **ou_fmt3)
{
    if (in_fmt1 && *in_fmt1) {
        avformat_close_input(in_fmt1);
        *in_fmt1 = NULL;
    }
    if (in_fmt2 && *in_fmt2) {
        avformat_close_input(in_fmt2);
        *in_fmt2 = NULL;
    }
    if (ou_fmt3 && *ou_fmt3 != NULL) {
        avformat_free_context(*ou_fmt3);
        *ou_fmt3 = NULL;
    }
}

void Muxing::initMuxing(char *inputfilepath_v, char *inputfilepath_a, char *outputpath) {

    //Input
    if ((ret = avformat_open_input(&audio_fmtCtx, inputfilepath_a, 0, 0)) < 0) {
        LOGE(MUXING_TAG, "initMuxing, Could not open input file.");
        return;
    }
    if ((ret = avformat_find_stream_info(audio_fmtCtx, 0)) < 0) {
        LOGE(MUXING_TAG, "initMuxing, Failed to retrieve input audio stream information: %s", av_err2str(ret));
        releaseContext(&audio_fmtCtx, NULL, NULL);
        return;
    }
    for (int i= 0;i<audio_fmtCtx->nb_streams;i++) {
        AVStream *stream = audio_fmtCtx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }

    if ((ret = avformat_open_input(&video_fmtCtx, inputfilepath_v, 0, 0)) < 0) {
        LOGE(MUXING_TAG, "initMuxing, Could not open input file.");
        releaseContext(&audio_fmtCtx, &video_fmtCtx, NULL);
        return;
    }
    if ((ret = avformat_find_stream_info(video_fmtCtx, 0)) < 0) {
        LOGE(MUXING_TAG, "initMuxing, Failed to retrieve input video stream information: %s", av_err2str(ret));
        releaseContext(&audio_fmtCtx, &video_fmtCtx, NULL);
        return;
    }
    for (int i=0;i<video_fmtCtx->nb_streams;i++) {
        AVStream *stream = video_fmtCtx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    LOGD(MUXING_TAG, "===========Input Information==========\n");
    av_dump_format(audio_fmtCtx, 0, inputfilepath_a, 0);
    av_dump_format(video_fmtCtx, 0, inputfilepath_v, 0);
    LOGD(MUXING_TAG, "======================================\n");

    //Output
    avformat_alloc_output_context2(&out_fmtCtx, NULL, NULL, outputpath);
    if (!out_fmtCtx) {
        LOGE(MUXING_TAG, "initMuxing, Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        releaseContext(&audio_fmtCtx, &video_fmtCtx, NULL);
        return;
    }
    if (audio_stream_index != -1) {
        audio_ou_stream = avformat_new_stream(out_fmtCtx,NULL);
        audio_in_stream = audio_fmtCtx->streams[audio_stream_index];
        if(avcodec_parameters_copy(audio_ou_stream->codecpar,audio_in_stream->codecpar) < 0){
            releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
            return;
        }
        audio_ou_stream->time_base = audio_in_stream->time_base;
    }
    if (video_stream_index != -1) {
        video_ou_stream = avformat_new_stream(out_fmtCtx,NULL);
        video_in_stream = video_fmtCtx->streams[video_stream_index];
        if (avcodec_parameters_copy(video_ou_stream->codecpar,video_in_stream->codecpar) < 0) {
            releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
            return;
        }
        video_ou_stream->time_base = video_in_stream->time_base;
    }

    LOGD(MUXING_TAG, "initMuxing, ==========Output Information==========\n");
    av_dump_format(out_fmtCtx, 0, outputpath, 1);
    LOGD(MUXING_TAG, "initMuxing, ======================================\n");
    //Open output file
    if (!(out_fmtCtx->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_fmtCtx->pb, outputpath, AVIO_FLAG_WRITE) < 0) {
            LOGE(MUXING_TAG, "initMuxing, Could not open output file '%s'", outputpath);
            releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
            return;
        }
    }
}


void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    LOGD(MUXING_TAG, "%s log_packet, pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           tag,
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}


void writePTS(AVPacket *pkt, AVStream *in_stream, int frame_index) {
    //FIX：No PTS (Example: Raw H.264)
    //Simple Write PTS
    if(pkt->pts == AV_NOPTS_VALUE){
        //Write PTS
        AVRational time_base1 = in_stream->time_base;
        //Duration between 2 frames (us)
        int64_t calc_duration = (double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
        //Parameters
        pkt->pts = (double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
        LOGD(MUXING_TAG, "writePTS,  frame_index = %d, pts = %d, calc_duration = %d", frame_index, pkt->pts, calc_duration);
        pkt->dts = pkt->pts;
        pkt->duration = (double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
    }
}


void Muxing::doMuxing() {
    if ((ret = avformat_write_header(out_fmtCtx,NULL)) < 0){
        LOGD(MUXING_TAG, "avformat_write_header %d",ret);
        releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
        return;
    }

    AVPacket *aSPacket = av_packet_alloc();
    AVPacket *vSPacket = av_packet_alloc();
    bool audio_finish = false,video_finish = false;
    bool found_audio = false,found_video = false;
    int64_t org_audio_pts = 0, org_video_pts = 0;
    do {
        if (!audio_finish && !found_audio && audio_stream_index != -1) {
            if (av_read_frame(audio_fmtCtx,aSPacket)< 0) {
                audio_finish = true;
                LOGD(MUXING_TAG, "audio_finish");
            }
            // 不是所需的音频数据
            if (!audio_finish && /**found_audio && */aSPacket->stream_index != audio_stream_index) {
                av_packet_unref(aSPacket);
                continue;
            }

            if (!audio_finish) {
                found_audio = true;
                writePTS(aSPacket, audio_in_stream, audio_frame_index);
                audio_frame_index++;

                // 改变packet的pts,dts,duration
                aSPacket->stream_index = audio_ou_stream->index;
                aSPacket->pts = av_rescale_q_rnd(org_audio_pts,audio_in_stream->time_base,audio_ou_stream->time_base,AV_ROUND_UP);
                aSPacket->dts = av_rescale_q_rnd(org_audio_pts,audio_in_stream->time_base,audio_ou_stream->time_base,AV_ROUND_UP);
                org_audio_pts += aSPacket->duration;
                aSPacket->duration = av_rescale_q_rnd(aSPacket->duration,audio_in_stream->time_base,audio_ou_stream->time_base,AV_ROUND_UP);
                aSPacket->pos = -1;
                // 要写入的packet的stream_index必须要设置正确
                aSPacket->stream_index = audio_ou_stream->index;

                AVRational tb = audio_in_stream->time_base;
            }
        }

        if (!video_finish && !found_video && video_stream_index != -1) {
            if (av_read_frame(video_fmtCtx,vSPacket) < 0) {
                video_finish = true;
                LOGD(MUXING_TAG, "video_finish");
            }
            if (!video_finish && /**found_video && */vSPacket->stream_index != video_stream_index) {
                av_packet_unref(vSPacket);
                continue;
            }

            if (!video_finish) {
                found_video = true;
                writePTS(vSPacket, video_in_stream, video_frame_index);
                video_frame_index++;
                /** 遇到问题：写入视频的帧率信息不正确
                 *  分析原因：avformat_parameters_copy()只是将编码参数进行了赋值，再进行封装时，帧率，视频时长是根据AVPacket的pts,dts,duration进行
                 *  计算的，所以这个时间就一定要和AVstream的time_base对应。
                 *  解决方案：进行如下的时间基的转换
                 */
                // 要写入的packet的stream_index必须要设置正确
                vSPacket->stream_index = video_ou_stream->index;
                vSPacket->pts = av_rescale_q_rnd(org_video_pts,video_in_stream->time_base,video_ou_stream->time_base,AV_ROUND_INF);
                vSPacket->dts = av_rescale_q_rnd(org_video_pts, video_in_stream->time_base, video_ou_stream->time_base, AV_ROUND_INF);
                org_video_pts += vSPacket->duration;
                vSPacket->duration = av_rescale_q_rnd(vSPacket->duration, video_in_stream->time_base, video_ou_stream->time_base, AV_ROUND_INF);
                aSPacket->pos = -1;
                AVRational tb = video_in_stream->time_base;
            }
        }

        // 写入数据 视频在前
        if (found_video && found_audio && !video_finish && !audio_finish) {
            int ts = av_compare_ts(aSPacket->pts,video_in_stream->time_base,
                              vSPacket->pts,audio_in_stream->time_base);
            LOGD(MUXING_TAG, "doMuxing, ts: %d", ts);
            if ((ret = ts) > 0) {
                // 写入视频
                log_packet(video_fmtCtx, vSPacket, "video: ");
                if ((ret = av_write_frame(out_fmtCtx,vSPacket)) < 0) {
                    LOGD(MUXING_TAG, "av_write_frame video 1 fail %d",ret);
                    releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
                    break;
                }
                LOGD(MUXING_TAG, "write video");
                found_video = false;
                av_packet_unref(vSPacket);
            } else {
                // 写入音频
                log_packet(audio_fmtCtx, aSPacket, "audio: ");
                if ((ret = av_write_frame(out_fmtCtx,aSPacket)) < 0) {
                    LOGD(MUXING_TAG, "av_write_frame audio 1 fail %d",ret);
                    releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
                    break;
                }
                LOGD(MUXING_TAG, "write audio");
                found_audio = false;
                av_packet_unref(aSPacket);
            }
        } else if (!video_finish && found_video && !found_audio){
            // 写入视频
            log_packet(video_fmtCtx, vSPacket, "video: found_video");
            if ((ret = av_write_frame(out_fmtCtx,vSPacket)) < 0) {
                LOGD(MUXING_TAG, "av_write_frame video 2 fail %d",ret);
                releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
                break;
            }
            LOGD(MUXING_TAG, "write found video");
            found_video = false;
            av_packet_unref(vSPacket);
        } else if (!found_video && found_audio && !audio_finish) {
            // 写入音频
            log_packet(audio_fmtCtx, aSPacket, "audio: found_audio");
            if ((ret = av_write_frame(out_fmtCtx,aSPacket)) < 0) {
                LOGD(MUXING_TAG, "av_write_frame audio 2 fail %d",ret);
                break;
            }
            LOGD(MUXING_TAG, "write found audio");
            found_audio = false;
            av_packet_unref(aSPacket);
        }
    } while(!audio_finish || !video_finish);


    // write tarailer
    av_write_trailer(out_fmtCtx);

    // release
    releaseContext(&audio_fmtCtx, &video_fmtCtx, &out_fmtCtx);
}