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
//    FILE *pcmFile = fopen(pcmFilename, "rb+");
    ifstream pcmFile(pcmFilename, ios::in | ios::binary);
    if (!pcmFile) {
        cout << "文件打开失败" << pcmFilename << endl;
        return;
    }
    
//    fseek(pcmFile, 0, SEEK_END);
    //获取文件大小
//    long size = ftell(pcmFile);
    //seek到文件头
//    fseek(pcmFile, 0, SEEK_SET);
    pcmFile.seekg(0, pcmFile.end);
    size_t size = pcmFile.tellg();
    pcmFile.seekg(0, pcmFile.beg);
    header.dataChunkDataSize = size;
    header.riffChunkDataSize = header.dataChunkDataSize
                                   + sizeof (WAVHeader)
                                    - sizeof (header.riffChunkId)
                                    - sizeof (header.riffChunkDataSize);

    // 打开wav文件
//    FILE *waveFile = fopen(wavFilename, "w+");
//    if (!waveFile) {
//        cout << "文件打开失败" << wavFilename << endl;
//        fclose(pcmFile);
//        return;
//    }
    ofstream waveFile(wavFilename, ios::out | ios::binary);
    // 写入头部
    waveFile.write((const char *) &header, sizeof (WAVHeader));
//    fwrite((const char *) &header, sizeof (WAVHeader), 1, waveFile);
    
    // 写入pcm数据
    char buf[1024];
//    size_t pcmSize = 0;
//    while ((pcmSize = fread(buf, 1, 1024, pcmFile)) > 0) {
//        fwrite(buf, pcmSize, 1, waveFile);
//    }
    while (pcmFile.read(buf, sizeof(buf))) {
        waveFile.write(buf, sizeof(buf));
    }
    pcmFile.close();
    waveFile.close();
    // 关闭文件
//    fclose(pcmFile);
//    fclose(waveFile);
}


