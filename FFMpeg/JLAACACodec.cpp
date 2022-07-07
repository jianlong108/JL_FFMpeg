//
//  JLAACACodec.cpp
//  FFMpeg
//
//  Created by Wangjianlong on 2022/1/15.
//

#include "JLAACACodec.hpp"
#include <iostream>
#include <fstream>

using namespace std;

// 检查编码器codec是否支持采样格式sample_fmt
static int check_sample_fmt(const AVCodec *codec,
                            enum AVSampleFormat sample_fmt) {
    const enum AVSampleFormat *p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt) return 1;
        p++;
    }
    return 0;
}

// 音频编码
// 返回负数：中途出现了错误
// 返回0：编码操作正常完成
static int encode(AVCodecContext *ctx,
                  AVFrame *frame,
                  AVPacket *pkt,
                  ofstream &outFile) {
    // 发送数据到编码器
    int ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "avcodec_send_frame error" << errbuf;
        return ret;
    }

    while (true) {
        // 从编码器中获取编码后的数据
        ret = avcodec_receive_packet(ctx, pkt);
        // packet中已经没有数据，需要重新发送数据到编码器（send frame）
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) { // 出现了其他错误
            ERROR_BUF(ret);
            cout << "avcodec_receive_packet error" << errbuf;
            return ret;
        }

        // 将编码后的数据写入文件
        outFile.write((char *) pkt->data, pkt->size);

        // 释放资源
        av_packet_unref(pkt);
    }

    return 0;
}



void JLAACCodec::aacEncode(AudioEncodeSpec &in,
                      const char *outFilename)
{
    
    // 上下文
    AVCodecContext *ctx = nullptr;

    // 用来存放编码前的数据
    AVFrame *frame = nullptr;
    // 用来存放编码后的数据
    AVPacket *pkt = nullptr;
    
    //设置编码器的属性
//    AVDictionary *options = nullptr;
    
    // 返回结果
    int ret = 0;

    //源文件
    ifstream inPcmFile(in.filename, ios::in | ios::binary);
    //目标文件
    ofstream outAACFile(outFilename, ios::out | ios::binary);

    const AVCodec *codec1 = avcodec_find_encoder(AV_CODEC_ID_AAC);

    const AVCodec *codec2 = avcodec_find_encoder_by_name("aac");

    // true
    cout << (codec1 == codec2) << endl;

    // aac
    cout << codec1->name << endl;
    // 获取fdk-aac编码器
    // 编码器
    const AVCodec *codec = avcodec_find_encoder_by_name("libfdk_aac");
    if (!codec) {
        cout << "encoder libfdk_aac not found";
        return;
    }
    // 检查采样格式
    if (!check_sample_fmt(codec, in.sampleFmt)) {
        cout << "Encoder does not support sample format"
                 << av_get_sample_fmt_name(in.sampleFmt);
        return;
    }

    // 创建上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        cout << "avcodec_alloc_context3 error";
        return;
    }

    // 设置参数
    ctx->sample_fmt = in.sampleFmt;
    ctx->sample_rate = in.sampleRate;
    ctx->channel_layout = in.chLayout;
    // 比特率
    ctx->bit_rate = 32000;
    // 规格
    ctx->profile = FF_PROFILE_AAC_HE_V2;
    
    // 打开编码器
    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "avcodec_open2 error" << errbuf;
        goto end;
    }
    
    //如果是想设置一些libfdk_aac特有的参数（比如vbr），可以通过options参数传递。
    //设置编码器的属性
//    av_dict_set(&options, "vbr", "1", 0);
//    ret = avcodec_open2(ctx, codec, &options);
    
    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        cout << "av_frame_alloc error";
        goto end;
    }

    // 样本帧数量（由frame_size决定）
    frame->nb_samples = ctx->frame_size;
    // 采样格式
    frame->format = ctx->sample_fmt;
    // 声道布局
    frame->channel_layout = ctx->channel_layout;
    // 创建AVFrame内部的缓冲区
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "av_frame_get_buffer error" << errbuf;
        goto end;
    }

    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        cout << "av_packet_alloc error";
        goto end;
    }
    // frame->linesize[0]是缓冲区的大小
    // 读取文件数据
    while ((ret = (int)inPcmFile.read((char *) frame->data[0], frame->linesize[0]).gcount()) > 0) {
        // 最后一次读取文件数据时，有可能并没有填满frame的缓冲区
        if (ret < frame->linesize[0]) {
            // 声道数
            int chs = av_get_channel_layout_nb_channels(frame->channel_layout);
            // 每个样本的大小
            int bytes = av_get_bytes_per_sample((AVSampleFormat) frame->format);
            // 改为真正有效的样本帧数量
            frame->nb_samples = ret / (chs * bytes);
        }

        // 编码
        if (encode(ctx, frame, pkt, outAACFile) < 0) {
            goto end;
        }
    }

    // flush编码器
    encode(ctx, nullptr, pkt, outAACFile);

end:
    // 关闭文件
    inPcmFile.close();
    outAACFile.close();

    // 释放资源
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&ctx);


}

#pragma mark -解码

static int decode(AVCodecContext *ctx,
                  AVPacket *pkt,
                  AVFrame *frame,
                  ofstream &outFile) {
    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "avcodec_send_packet error" << errbuf << endl;
        return ret;
    }
 
    while (true) {
        // 获取解码后的数据
        ret = avcodec_receive_frame(ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            ERROR_BUF(ret);
            cout << "avcodec_receive_frame error" << errbuf<< endl;
            return ret;
        }
        // 将解码后的数据写入文件
        outFile.write((char *) frame->data[0], frame->linesize[0]);
    }
}

void JLAACCodec::aacDecode(const char *inFilename,
                      AudioEncodeSpec &out)
{
    // 输入缓冲区的大小
    #define IN_DATA_SIZE 20480
    // 需要再次读取输入文件数据的阈值
    #define REFILL_THRESH 4096
    // 返回结果
    int ret = 0;
    // 每次从输入文件中读取的长度
    long inLen = 0;
    // 是否已经读取到了输入文件的尾部
    int inEnd = 0;
    // 用来存放读取的文件数据
    //加上AV_INPUT_BUFFER_PADDING_SIZE是为了防止某些优化过的reader一次性读取过多导致越界
    char inDataArray[IN_DATA_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    char *inData = inDataArray;
   
    //源文件
    ifstream inFile(inFilename, ios::in | ios::binary);
    //目标文件
    ofstream outFile(out.filename, ios::out | ios::binary);
    
    // 解码器
    const AVCodec *codec = nullptr;
    // 上下文
    AVCodecContext *ctx = nullptr;
    // 解析器上下文
    AVCodecParserContext *parserCtx = nullptr;
    // 存放解码前的数据
    AVPacket *pkt = nullptr;
    // 存放解码后的数据
    AVFrame *frame = nullptr;
    
    
    // 获取解码器
    codec = avcodec_find_decoder_by_name("libfdk_aac");
    if (!codec) {
        cout << "decoder libfdk_aac not found" << endl;
        return;
    }
    
    // 初始化解析器上下文
    parserCtx = av_parser_init(codec->id);
    if (!parserCtx) {
        cout << "av_parser_init error"<< endl;
        return;
    }
    
    // 创建上下文
    ctx = avcodec_alloc_context3(codec);
    if (!ctx) {
        cout << "avcodec_alloc_context3 error"<< endl;
        goto end;
    }
    
    // 创建AVPacket
    pkt = av_packet_alloc();
    if (!pkt) {
        cout << "av_packet_alloc error"<< endl;
        goto end;
    }
    
    // 创建AVFrame
    frame = av_frame_alloc();
    if (!frame) {
        cout << "av_frame_alloc error"<< endl;
        goto end;
    }
    
    // 打开解码器
    ret = avcodec_open2(ctx, codec, nullptr);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "avcodec_open2 error" << errbuf << endl;;
        goto end;
    }
    
    // 打开文件
    
    // 读取数据
    inLen = inFile.read(inData, IN_DATA_SIZE).gcount();
    while (inLen > 0) {
        // 经过解析器上下文处理
        ret = av_parser_parse2(parserCtx, ctx,
                               &pkt->data, &pkt->size,
                               (uint8_t *) inData, (int)inLen,
                               AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            ERROR_BUF(ret);
            cout << "av_parser_parse2 error" << errbuf << endl;
            goto end;
        }
        // 跳过已经解析过的数据
        inData += ret;
        // 减去已经解析过的数据大小
        inLen -= ret;
     
        // 解码
        if (pkt->size > 0 && decode(ctx, pkt, frame, outFile) < 0) {
            goto end;
        }
     
        // 如果数据不够了，再次读取文件
        if (inLen < REFILL_THRESH && !inEnd) {
            // 剩余数据移动到缓冲区前
            memmove(inDataArray, inData, inLen);
            inData = inDataArray;
     
            // 跨过已有数据，读取文件数据
            long len = inFile.read(inData + inLen, IN_DATA_SIZE - inLen).gcount();
            if (len > 0) {
                inLen += len;
            } else {
                inEnd = 1;
            }
        }
    }
     
    // flush解码器
    //    pkt->data = NULL;
    //    pkt->size = 0;
    decode(ctx, nullptr, frame, outFile);
    
end:
    inFile.close();
    outFile.close();
    av_frame_free(&frame);
    av_packet_free(&pkt);
    av_parser_close(parserCtx);
    avcodec_free_context(&ctx);

}
