//
//  JLAudioResample.hpp
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/26.
//

#ifndef JLAudioResample_hpp
#define JLAudioResample_hpp

#include <stdio.h>
// 导入头文件
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
}

// 音频参数
typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
} ResampleAudioSpec;

class JLAudioResample {
public:
    static void resampleAudio(ResampleAudioSpec &in,
                              ResampleAudioSpec &out);

    static void resampleAudio(const char *inFilename,
                              int inSampleRate,
                              AVSampleFormat inSampleFmt,
                              int inChLayout,

                              const char *outFilename,
                              int outSampleRate,
                              AVSampleFormat outSampleFmt,
                              int outChLayout);
};

#endif /* JLAudioResample_hpp */
