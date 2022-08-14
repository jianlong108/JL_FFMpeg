//
//  YUVCore.cpp
//  FFMpeg
//
//  Created by JL on 2022/8/11.
//

#include "YUVCore.hpp"
#include <fstream>
#include "FFmpegTool.hpp"

using namespace::std;

YUVCore::YUVCore()
{
    cout << "YUVCore() width : " << this->_currentItem
    << " height : " << this->_currentItem
    << endl;
}

YUVCore::~YUVCore()
{
    cout << "~YUVCore()" << endl;
}

bool YUVCore::isPlaying()
{
    return _currentState == Playing;
}

void YUVCore::play()
{
    if (_currentState == Playing) {
        return;
    }
    
    //绘制一帧
    
    setPlayState(Playing);
}

void YUVCore::pause()
{
    if (_currentState != Playing) {
        return;
    }
    
    //
    
    setPlayState(Paused);
}

void YUVCore::stop()
{
    if (_currentState == Stopped) {
        return;
    }
    
    
    setPlayState(Stopped);
}

void YUVCore::setPlayState(State s)
{
    if (s == _currentState) {
        return;
    }
    if (s == Stopped || s == Finished) {
        _file->seekg(0, _file->beg);
    }
    _currentState = s;
    //回调上层
    m_callBackfunc(_currentState);
}

YUVCore::State YUVCore::getState() {
    return _currentState;
}

YUVItem* YUVCore::getCurrentYUVItem()
{
    return _currentItem;
}

void YUVCore::setYUVItem(YUVItem *item)
{
    _currentItem = item;
    //关闭上个文件
    closeFile();
    
    _file = new ifstream(_currentItem->fileName, ios::in | ios::binary);
    if (!_file) return;
    _imgSize = av_image_get_buffer_size(_currentItem->pixelFormat, _currentItem->width, _currentItem->height, 1);
    
    cout << "文件路径:" << _currentItem->fileName
    << " 是否打开=" <<_file->is_open()
    << " 每帧图片的大小: " <<  _imgSize
    << " 像素格式=" << av_get_pix_fmt_name( _currentItem->pixelFormat)
    << " 宽:高 = " <<  _currentItem->width << ":" << _currentItem->height
    << endl;
}

void YUVCore::closeFile()
{
    if (!_file) {
        return;
    }
    _file->close();
    delete _file;
    _file = nullptr;
}

char * YUVCore::getOneFrameRawDataOfRGB24(int *error)
{
    if (!_file->is_open()) {
        return nullptr;
    }
    //tellg() 读取输入流中文件指针的位置，返回值可转化为 int。
    // 图片大小
    char data[_imgSize];
    size_t realSize = 0;
    if ((realSize = _file->read(data, _imgSize).gcount()) == _imgSize) {
        RawVideoFrame in = {
            data,
            _currentItem->width, _currentItem->height,
            _currentItem->pixelFormat
        };
        RawVideoFrame out = {
            nullptr,
            _currentItem->width >> 4 << 4,
            _currentItem->height >> 4 << 4,
            AV_PIX_FMT_RGB24
        };
        FFmpegTool::convertRawVideo(in, out);
        
        *error = 0;
        return out.pixels;
    } else { // 文件数据已经读取完毕
        cout << "读取的大小: " << realSize << " 需要一帧的大小: " << _imgSize << endl;
        // 停止定时器
        *error = 1;
        // 正常播放完毕
        setPlayState(Finished);
        return nullptr;
    }
//    if (_file->read(data, _imgSize).gcount() == _imgSize) {
//        RawVideoFrame in = {
//            data,
//            _currentItem->width, _currentItem->height,
//            _currentItem->pixelFormat
//        };
//        RawVideoFrame out = {
//            nullptr,
//            _currentItem->width >> 4 << 4,
//            _currentItem->height >> 4 << 4,
//            AV_PIX_FMT_RGB24
//        };
//        FFmpegTool::convertRawVideo(in, out);
//
//        *imgData = out.pixels;
//        *error = 0;
//    } else { // 文件数据已经读取完毕
//        // 停止定时器
//        *error = 1;
//        // 正常播放完毕
//        setPlayState(Finished);
//    }
}

void YUVCore::setStateChangeCallBack(stateChangeCallBack callBackFunc)
{
    m_callBackfunc = callBackFunc;
}

bool YUVCore::canPlay()
{
    return this->_currentItem;
}
