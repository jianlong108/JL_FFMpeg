//
//  RecordPCM.cpp
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/21.
//

#include "JLRecordPCM.hpp"
#include <iostream>

extern "C" {
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


using namespace::std;
void showSpec(AVFormatContext *ctx);


void JLRecordPCM::beginRecord()
{
    AVInputFormat *inFmt = av_find_input_format("avfoundation");
    if (!inFmt) {
        cout << "找不到输入格式: avfoundation" << endl;
        return;
    }
    AVFormatContext *ctx = nullptr;//avformat_alloc_context();
//        AVDictionary *options = nil;
//    av_dict_set(&options, "framerate", "30", 0);
    int ret = avformat_open_input(&ctx,":1",inFmt,NULL);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        cout << "avformat_open_input error:" << errbuf <<endl;
        return;
    }
    showSpec(ctx);
    // 获取当前时间，精确到秒
    time_t currentTime = std::time(NULL);
    char chCurrentTime[128];
    strftime(chCurrentTime, sizeof(chCurrentTime), "%Y-%m-%d_%H-%M-%S", std::localtime(&currentTime)); //年月日 时分秒
    string stCurrentTime = chCurrentTime;// 转为string
    string fileName = "/Users/dalong/Desktop/jl_" + stCurrentTime + ".pcm";// 示例
    FILE *output_fd = fopen(fileName.c_str(), "wb+");
    assert(output_fd);
    // 数据包
    AVPacket *pkt = av_packet_alloc();
    pkt->data = NULL;
    pkt->size = 0;
    while (!this->stopRecord) {
        ret = av_read_frame(ctx, pkt);
        if (ret < 0) {
            if (ret == -35) {
                continue;
            }

            break;
        }
        fwrite(pkt->data, pkt->size, 1, output_fd);
        fflush(output_fd);
        av_packet_unref(pkt);
    }

    av_packet_free(&pkt);
    avformat_close_input(&ctx);
    cout << "录音结束" << endl;
}

void showSpec(AVFormatContext *ctx) {
    // 获取输入流
    AVStream *stream = ctx->streams[0];
    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;
    // 声道数
    cout << params->channels << endl;
    // 采样率
    
    cout << params->sample_rate << endl;
    // 采样格式
    cout << params->format << endl;
    // 每一个样本的一个声道占用多少个字节
    cout << av_get_bytes_per_sample((AVSampleFormat) params->format) << endl;
}