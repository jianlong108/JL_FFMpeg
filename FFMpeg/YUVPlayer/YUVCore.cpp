//
//  YUVCore.cpp
//  FFMpeg
//
//  Created by JL on 2022/8/11.
//

#include "YUVCore.hpp"
#include <fstream>
#include "FFmpegTool.hpp"

using namespace std;

YUVCore::YUVCore()
{
    std::cout << "YUVCore() width : " << this->_currentItem.width
    << " height : " << this->_currentItem.height
    << std::endl;
}

YUVCore::~YUVCore()
{
    std::cout << "~YUVCore()" << std::endl;
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
//        fseek(_file, 0, SEEK_SET);
    }
    _currentState = s;
}

YUVCore::State YUVCore::getState() {
    return _currentState;
}

void YUVCore::setYUVItem(YUVItem &item)
{
    _currentItem = item;
    //关闭上个文件
    closeFile();
    std::cout << "打开对应文件" << std::endl;
//    _file = fopen(_currentItem.fileName, "r");
    
    _file = new ifstream(_currentItem.fileName, ios::in | ios::binary);
    
//    std::ifstream std::ifstream(_currentItem.fileName, std::ios::in | std::ios::binary);
    _imgSize = av_image_get_buffer_size(_currentItem.pixelFormat, _currentItem.width, _currentItem.height, 1);
    
}

void YUVCore::closeFile()
{
    if (!_file) {
        return;
    }
    _file->close();
//    fclose(_file);
    delete _file;
    _file = nullptr;
}

void YUVCore::getImageDataFromOneFrame(char *imgData, int *error)
{
    if (!_file->is_open()) {
        return;
    }
    // 图片大小
    char data[_imgSize];
    if (!_file->eof() && _file->read(data, sizeof(data))) {
        RawVideoFrame in = {
            data,
            _currentItem.width, _currentItem.height,
            _currentItem.pixelFormat
        };
        RawVideoFrame out = {
            nullptr,
            _currentItem.width >> 4 << 4,
            _currentItem.height >> 4 << 4,
            AV_PIX_FMT_RGB24
        };
        FFmpegTool::convertRawVideo(in, out);
        
        imgData = out.pixels;
//        freeCurrentImage();
//        _currentImage = new QImage((uchar *) out.pixels,
//                                   out.width, out.height, QImage::Format_RGB888);
        *error = 0;
        // 刷新
//        update();
    } else { // 文件数据已经读取完毕
        // 停止定时器
//        stopTimer();
        *error = 1;
        // 正常播放完毕
        setPlayState(Finished);
    }
}

void YUVCore::freeCurrentImage()
{
    
}
