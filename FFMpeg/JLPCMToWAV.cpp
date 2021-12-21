//
//  JLRecordWav.cpp
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/21.
//

#include "JLPCMToWAV.hpp"
#include <fstream>

using namespace::std;

void JLPCMToWAV::pcm2wav(WAVHeader &header,
                    const char *pcmFilename,
                    const char *wavFilename)
{
    
    header.blockAlign = header.bitsPerSample * header.numChannels >> 3;
    header.byteRate = header.sampleRate * header.blockAlign;
    
    // 打开pcm文件
    FILE *pcmFile = fopen(pcmFilename, "rb+");
    if (!pcmFile) {
        cout << "文件打开失败" << pcmFilename << endl;
        return;
    }
    
    fseek(pcmFile, 0, SEEK_END);
    size_t size = ftell(pcmFile);
    //seek到文件头
    fseek(pcmFile, 0, SEEK_SET);
    header.dataChunkDataSize = size;
    header.riffChunkDataSize = header.dataChunkDataSize
                                   + sizeof (WAVHeader) - 8;

    // 打开wav文件
    FILE *waveFile = fopen(wavFilename, "wb+");
    if (!waveFile) {
        cout << "文件打开失败" << wavFilename << endl;
        fclose(pcmFile);
        return;
    }
    // 写入头部
    fwrite((const char *) &header, sizeof (WAVHeader), 1, waveFile);
    
    // 写入pcm数据
    char buf[1024];
    size_t pcmSize = 0;
    while ((pcmSize = fread(buf, 1, 1024, pcmFile)) > 0) {
        fwrite(buf, pcmSize, 1, waveFile);
    }

    // 关闭文件
    fclose(pcmFile);
    fclose(waveFile);
}


