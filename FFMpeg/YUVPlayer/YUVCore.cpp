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

static void timerEvent(void *context) {
    
}
static void cancelTimerEvent(void *context) {
    
}



YUVCore::YUVCore()
{
    cout << "YUVCore() width : " << this->_currentItem.width
    << " height : " << this->_currentItem.height
    << " pixelFormat : " << av_get_pix_fmt_name(this->_currentItem.pixelFormat)
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
    
    // 启动定时器
    // dispatch_resume(m_timer);
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

YUVItem& YUVCore::getCurrentYUVItem()
{
    return _currentItem;
}

void YUVCore::setYUVItem(YUVItem &item)
{
    _currentItem = item;
    //关闭上个文件
    closeFile();
    
    _file = new ifstream(_currentItem.fileName, ios::in | ios::binary);
    if (!_file) return;
    _imgSize = av_image_get_buffer_size(_currentItem.pixelFormat, _currentItem.width, _currentItem.height, 1);
    
    cout << "文件路径:" << _currentItem.fileName
    << " 是否打开=" <<_file->is_open()
    << " 每帧图片的大小: " <<  _imgSize
    << " 像素格式=" << av_get_pix_fmt_name( _currentItem.pixelFormat)
    << " 宽:高 = " <<  _currentItem.width << ":" << _currentItem.height
    << endl;
    /*
    if (!m_timer) {
        m_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
         
        // 定时任务调度设置,0秒后启动,每个5秒运行
        dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW ,0);
        
         //dispatch_source_set_timer(dispatch_source_t source,
         //    dispatch_time_t start, //开始时间
         //    uint64_t interval, // 间隔时间
         //    uint64_t leeway); //可接受误差 纳秒 填0 == 不接受误差
        
        dispatch_source_set_timer(m_timer, time, (int)1000/_currentItem.fps * NSEC_PER_MSEC, 0 * NSEC_PER_SEC);
        
//        __weak typeof(self) weakSelf = self;
//        dispatch_source_set_event_handler(timer, ^{
            // 定时任务
//            @autoreleasepool {
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [weakSelf drawView];
//                });
//            }
//        });
        dispatch_source_set_event_handler_f(m_timer, timerEvent);
        
        dispatch_source_set_cancel_handler_f(m_timer, cancelTimerEvent);
        
//        m_timer = timer;
         
    }
*/
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

void YUVCore::setPushOneFrameRawDataCallBack(pushOneFrameRawDataFunc callBackFunc)
{
    m_frameDataFunc = callBackFunc;
}

bool YUVCore::canPlay()
{
    return this->_currentItem.vaild();
}
