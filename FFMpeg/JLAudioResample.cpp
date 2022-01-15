//
//  JLAudioResample.cpp
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/26.
//

#include "JLAudioResample.hpp"
#include <iostream>
#include <fstream>


using namespace::std;

// 处理错误码
#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

void JLAudioResample::resampleAudio(ResampleAudioSpec &in,
                            ResampleAudioSpec &out)
{
    resampleAudio(in.filename, in.sampleRate, in.sampleFmt, in.chLayout,
                  out.filename, out.sampleRate, out.sampleFmt, out.chLayout);
}

void JLAudioResample::resampleAudio(const char *inFilename, int inSampleRate, AVSampleFormat inSampleFmt, int inChLayout, const char *outFilename, int outSampleRate, AVSampleFormat outSampleFmt, int outChLayout)
{
    //源文件
    ifstream inPcmFile(inFilename, ios::in | ios::binary);
    //目标文件
    ofstream outPcmFile(outFilename, ios::out | ios::binary);
    
    // 输入缓冲区
    // 指向缓冲区的指针
    uint8_t **inData = nullptr;
    // 缓冲区的大小
    int inLinesize = 0;
    // 声道数
    int inChs = av_get_channel_layout_nb_channels(inChLayout);
    // 一个样本的大小
    int inBytesPerSample = inChs * av_get_bytes_per_sample(inSampleFmt);
    // 缓冲区的样本数量
    int inSamples = 1024;
    // 读取文件数据的大小
    long len = 0;

    // 输出缓冲区
    // 指向缓冲区的指针
    uint8_t **outData = nullptr;
    // 缓冲区的大小
    int outLinesize = 0;
    // 声道数
    int outChs = av_get_channel_layout_nb_channels(outChLayout);
    // 一个样本的大小
    int outBytesPerSample = outChs * av_get_bytes_per_sample(outSampleFmt);
    // 缓冲区的样本数量（AV_ROUND_UP是向上取整）
    int outSamples = av_rescale_rnd(outSampleRate, inSamples, inSampleRate, AV_ROUND_UP);

    /*
     inSampleRate     inSamples
     ------------- = -----------
     outSampleRate    outSamples

     outSamples = outSampleRate * inSamples / inSampleRate
     */
    int ret = 0;
    // 创建重采样上下文
    SwrContext *ctx = swr_alloc_set_opts(nullptr,
                                         // 输出参数
                                         outChLayout, outSampleFmt, outSampleRate,
                                         // 输入参数
                                         inChLayout, inSampleFmt, inSampleRate,
                                         0, nullptr);
    if (!ctx) {
        cout << "swr_alloc_set_opts error";
        goto end;
    }
    
    // 初始化重采样上下文
    ret = swr_init(ctx);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "swr_init error:" << errbuf;
        goto end;
    }
    
    // 创建输入缓冲区
    ret = av_samples_alloc_array_and_samples(
              &inData,
              &inLinesize,
              inChs,
              inSamples,
              inSampleFmt,
              1);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "av_samples_alloc_array_and_samples error:" << errbuf;
        goto end;
    }

    // 创建输出缓冲区
    ret = av_samples_alloc_array_and_samples(
              &outData,
              &outLinesize,
              outChs,
              outSamples,
              outSampleFmt,
              1);
    if (ret < 0) {
        ERROR_BUF(ret);
        cout << "av_samples_alloc_array_and_samples error:" << errbuf;
        goto end;
    }
    // 打开文件
    if (!inPcmFile) {
        cout << "file open error:" <<endl;
        goto end;
    }
    if (!outPcmFile) {
        cout << "file open error:" <<endl;
        goto end;
    }

    // 读取文件数据
    // inData[0] == *inData
    while ((len = inPcmFile.read((char *) inData[0], inLinesize).gcount()) > 0) {
        // 读取的样本数量
        inSamples = (int)(len / inBytesPerSample);

        // 重采样(返回值转换后的样本数量)
        ret = swr_convert(ctx,
                          outData, outSamples,
                          (const uint8_t **) inData,
                          inSamples
                         );

        if (ret < 0) {
            ERROR_BUF(ret);
            cout << "swr_convert error:" << errbuf;
            goto end;
        }

        // 将转换后的数据写入到输出文件中
        // outData[0] == *outData
        outPcmFile.write((char *) outData[0], ret * outBytesPerSample);
    }
    // 检查一下输出缓冲区是否还有残留的样本（已经重采样过的，转换过的）
    while ((ret = swr_convert(ctx,
                              outData, outSamples,
                              nullptr, 0)) > 0) {
        outPcmFile.write((char *) outData[0], ret * outBytesPerSample);
    }

    cout << "转换结束" << endl;
end:
    inPcmFile.close();
    outPcmFile.close();
    // 释放输入缓冲区
    if (inData) {
        av_freep(&inData[0]);
    }
    av_freep(&inData);

    // 释放输出缓冲区
    if (outData) {
        av_freep(&outData[0]);
    }
    av_freep(&outData);
    swr_free(&ctx);
}
