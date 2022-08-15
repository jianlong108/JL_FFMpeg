//
//  PlayPCM.cpp
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/21.
//

#include "JLPlayPCM.hpp"
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
typedef struct AudioBuffer {
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


void JLPlayPCM::beginPlay()
{
    // 初始化Audio子系统
    if (SDL_Init(SDL_INIT_AUDIO)) {
        // 返回值不是0，就代表失败
        cout << "SDL_Init Error " << SDL_GetError() << endl;
        return;
    }
    
    // 音频参数
    SDL_AudioSpec spec;
    // 采样率
    spec.freq = SAMPLE_RATE;
    // 采样格式
    spec.format = SAMPLE_FORMAT;
    // 声道数
    spec.channels = CHANNELS;
    // 音频缓冲区的样本数量（这个值必须是2的幂）
    spec.samples = SAMPLES;
    // 回调
    spec.callback = pull_audio_data;
    // 传递给回调的参数
    AudioBuffer buffer;
    spec.userdata = &buffer;

    // 打开音频设备
    if (SDL_OpenAudio(&spec, NULL)) {
        
        cout <<  "SDL_OpenAudio Error " << SDL_GetError() << endl;
        // 清除所有初始化的子系统
        SDL_Quit();
        return;
    }
    
    FILE *output_fd = fopen("/Users/dalong/Desktop/AV/3.pcm", "rb+");
    if (!output_fd) {
        cout  << "文件打开失败 " << endl;
        // 关闭音频设备
        SDL_CloseAudio();
        // 清除所有初始化的子系统
        SDL_Quit();
        return;
    }
    
    // 开始播放
    SDL_PauseAudio(0);

    // 存放文件数据
    Uint8 data[BUFFER_SIZE];

    while (!this->stopPlay) {
        // 只要从文件中读取的音频数据，还没有填充完毕，就跳过
        if (buffer.len > 0) continue;
        buffer.len = (int)fread(data, 1, BUFFER_SIZE, output_fd);
        
        // 文件数据已经读取完毕
        if (buffer.len <= 0) {
            // 剩余的样本数量
            int samples = buffer.pullLen / BYTES_PER_SAMPLE;
            int ms = samples * 1000 / SAMPLE_RATE;
            SDL_Delay(ms);
            break;
        }

        // 读取到了文件数据
        buffer.data = data;
    }
    // 关闭文件
    fclose(output_fd);
    // 关闭音频设备
    SDL_CloseAudio();
    // 清理所有初始化的子系统
    SDL_Quit();
}
