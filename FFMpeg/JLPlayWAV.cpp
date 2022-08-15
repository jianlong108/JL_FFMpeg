//
//  JLPlayWAV.cpp
//  FFMpeg
//
//  Created by Wangjianlong on 2022/1/8.
//

#include "JLPlayWAV.hpp"
#include <iostream>


extern "C" {
#include <SDL2/SDL.h>
}

using namespace::std;

// 采样率
#define SAMPLE_RATE 44100
// 采样格式
#define SAMPLE_FORMAT AUDIO_F32LSB
// 采样大小
#define SAMPLE_SIZE SDL_AUDIO_BITSIZE(SAMPLE_FORMAT)
// 声道数
#define CHANNELS 2

// 音频缓冲区的样本数量
#define SAMPLES 1024

// 每个样本占用多少个字节
#define BYTES_PER_SAMPLE ((SAMPLE_SIZE * CHANNELS) / 8)

// 文件缓冲区的大小
#define BUFFER_SIZE (SAMPLES * BYTES_PER_SAMPLE)


// 用于存储读取的音频数据和长度
typedef struct AudioBuffer
{
    int len = 0;
    int pullLen = 0;
    Uint8 *data = nullptr;
} AudioBuffer;


// userdata：SDL_AudioSpec.userdata
// stream：音频缓冲区（需要将音频数据填充到这个缓冲区）
// len：音频缓冲区的大小（SDL_AudioSpec.samples * 每个样本的大小）
static void pull_audio_data(void *userdata, Uint8 *stream, int len) {
    // 清空stream
    SDL_memset(stream, 0, len);

    // 取出缓冲信息
    AudioBuffer *buffer = (AudioBuffer *) userdata;
    if (buffer->len == 0) return;

    // 取len、bufferLen的最小值（为了保证数据安全，防止指针越界）
    buffer->pullLen = (len > buffer->len) ? buffer->len : len;
    
    // 填充数据
    SDL_MixAudio(stream,
                 buffer->data,
                 buffer->pullLen,
                 SDL_MIX_MAXVOLUME);
    buffer->data += buffer->pullLen;
    buffer->len -= buffer->pullLen;
}


void JLPlayWAV::beginPlay()
{
    // 初始化Audio子系统
    if (SDL_Init(SDL_INIT_AUDIO)) {
        // 返回值不是0，就代表失败
        cout << "SDL_Init Error " << SDL_GetError() << endl;
        return;
    }
    
    // 音频参数
    SDL_AudioSpec spec;

    // WAV中的PCM数据
    Uint8 *data;
    // WAV中的PCM数据大小（字节）
    Uint32 len;
    // 加载wav文件
    if (!SDL_LoadWAV("/Users/dalong/Desktop/AV/7.wav", &spec, &data, &len)) {
        cout << "SDL_LoadWAV error:" << SDL_GetError();
        // 清除所有的子系统
        SDL_Quit();
        return;
    }
    // 回调
    spec.callback = pull_audio_data;
    // 传递给回调函数的userdata
    AudioBuffer buffer;
    buffer.len = len;
    buffer.data = data;
    spec.userdata = &buffer;
    
    // 打开设备
    if (SDL_OpenAudio(&spec, nullptr)) {
        cout << "SDL_OpenAudio error:" << SDL_GetError();
        // 释放文件数据
        SDL_FreeWAV(data);
        // 清除所有的子系统
        SDL_Quit();
        return;
    }

    // 开始播放（0是取消暂停）
    SDL_PauseAudio(0);

    while (!this->stopPlay) {
        if (buffer.len > 0) continue;
        // 每一个样本的大小
        int size = spec.channels * SDL_AUDIO_BITSIZE(spec.format) / 8;
        // 最后一次播放的样本数量
        int samples = buffer.pullLen / size;
        // 最后一次播放的时长
        int ms = samples * 1000 / spec.freq;
        SDL_Delay(ms);
        break;
    }
    
    // 释放WAV文件数据
    SDL_FreeWAV(data);

    // 关闭设备
    SDL_CloseAudio();

    // 清除所有的子系统
    SDL_Quit();
}
