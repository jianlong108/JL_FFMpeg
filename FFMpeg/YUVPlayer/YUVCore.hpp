//
//  YUVCore.hpp
//  FFMpeg
//
//  Created by JL on 2022/8/11.
//

#ifndef YUVCore_hpp
#define YUVCore_hpp

#include <stdio.h>
#include <iostream>

extern "C" {
#include <libavutil/imgutils.h>
}


typedef struct {
    int width = 640;
    int height = 480;
    AVPixelFormat pixelFormat;
    const char *fileName;
    int fps;
} YUVItem;

class YUVCore {

public:
    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Finished
    } State;
    
    void play();
    void stop();
    void pause();
    bool isPlaying();

    void setYUVItem(YUVItem *item);
    
    State getState();
    
    YUVCore();
    ~YUVCore();
    
    char* getImageDataFromOneFrame(int *error);
    
private:
    /// 当前的播放状态
    State _currentState = Stopped;
    
    /// 当前播放的yuv
    YUVItem *_currentItem = nullptr;
    std::ifstream *_file = nullptr;
    
    /// 一帧图片的大小
    int _imgSize = 0;
    
    void setPlayState(State s);
    
    void closeFile();
    
    void freeCurrentImage();
//    FILE *_file = nullptr;
};

#endif /* YUVCore_hpp */
