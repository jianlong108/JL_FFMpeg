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

//#include <dispatch/dispatch.h>
//#include <dispatch/source.h>

extern "C" {
#include <libavutil/imgutils.h>
}

struct YUVItem {
    int width = 0;
    int height = 0;
    AVPixelFormat pixelFormat;
    const char *fileName;
    int fps;
    
    bool vaild() {
        return this->width > 0 && this->height > 0 && this->pixelFormat == AV_PIX_FMT_YUV420P;
    }
    
    YUVItem() {
        this->pixelFormat = AV_PIX_FMT_YUV420P;
        std::cout << "YUVItem()" << std::endl;
    };
    
    ~YUVItem() {
        std::cout << "~YUVItem()" << std::endl;
    };
};

class YUVCore {

public:
    typedef enum {
        Stopped = 0,
        Playing,
        Paused,
        Finished
    } State;
    
    typedef void(*stateChangeCallBack)(State state);
    typedef void(*pushOneFrameRawDataFunc)(char *, int, int);
    
    void play();
    void stop();
    void pause();
    bool isPlaying();

    void setYUVItem(YUVItem &item);
    YUVItem& getCurrentYUVItem();
    bool canPlay();
    void setStateChangeCallBack(stateChangeCallBack callBackFunc);
    void setPushOneFrameRawDataCallBack(pushOneFrameRawDataFunc callBackFunc);
    
    State getState();
    
    YUVCore();
    ~YUVCore();
    
    char* getOneFrameRawDataOfRGB24(int *error);
    
private:
    /// 一帧图片的大小
    int _imgSize = 0;
    
    /// 当前播放的yuv
    YUVItem _currentItem;
    std::ifstream *_file = nullptr;
    
    void closeFile();
//    dispatch_source_t m_timer;
    /// 当前的播放状态
    State _currentState = Stopped;
    
    void setPlayState(State s);
    stateChangeCallBack m_callBackfunc = nullptr;
    pushOneFrameRawDataFunc m_frameDataFunc = nullptr;
};

#endif /* YUVCore_hpp */
