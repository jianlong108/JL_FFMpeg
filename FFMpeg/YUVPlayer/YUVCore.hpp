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

struct YUVItem {
    int width = 0;
    int height = 0;
    AVPixelFormat pixelFormat;
    const char *fileName;
    int fps;
};

class YUVCore {

public:
    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Finished
    } State;
    
    typedef void(*stateChangeCallBack)(State state) ;
    
    void play();
    void stop();
    void pause();
    bool isPlaying();

    void setYUVItem(YUVItem *item);
    YUVItem * getCurrentYUVItem();
    bool canPlay();
    void setStateChangeCallBack(stateChangeCallBack callBackFunc);
    
    State getState();
    
    YUVCore();
    ~YUVCore();
    
    char* getOneFrameOfRawDataRGB24(int *error);
    
private:
    /// 一帧图片的大小
    int _imgSize = 0;
    
    /// 当前播放的yuv
    YUVItem *_currentItem = nullptr;
    std::ifstream *_file = nullptr;
    
    void closeFile();
    
    /// 当前的播放状态
    State _currentState = Stopped;
    
    void setPlayState(State s);
    stateChangeCallBack m_callBackfunc = nullptr;
};

#endif /* YUVCore_hpp */
