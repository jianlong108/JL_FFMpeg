//
//  YUV420PPlayer.m
//  FFMpeg
//
//  Created by JL on 2022/8/8.
//

#import "YUVMacOSPlayer.h"
#include "YUVCore.hpp"

@interface YUVMacOSPlayer ()
{
    YUVCore *_core;
//    CVDisplayLinkRef displayLink;
}

@property (nonatomic, assign) CGRect dstRect;

@property (nonatomic, strong) NSImageView *imgView;

@end

@implementation YUVMacOSPlayer

static dispatch_source_t g_timer = nil;

static void stateChange(YUVCore::State state)
{
    NSLog(@"stateChange: %d",state);
    
    if (state == YUVCore::Stopped) {
        dispatch_source_cancel(g_timer);
        g_timer = nil;
    } else if (state == YUVCore::Paused) {
        dispatch_suspend(g_timer);
    }
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect]) {
        [self _initlize];
    }
    return self;
}

- (void)_initlize
{
    self->_core = new YUVCore;
    
    self->_core->setStateChangeCallBack(stateChange);
    
    self.wantsLayer = YES;
    self.layer.backgroundColor = [NSColor redColor].CGColor;
    
    _imgView = [[NSImageView alloc] initWithFrame:self.bounds];
    _imgView.wantsLayer = YES;
    _imgView.layer.backgroundColor = [NSColor blueColor].CGColor;
    [self addSubview:_imgView];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    NSLog(@"YUVMacOSPlayer drawRect");
}

- (void)dealloc
{
    NSLog(@"YUVMacOSPlayer dealloc");
    delete self->_core;
//    if (displayLink) {
//        CVDisplayLinkRelease(displayLink);
//    }
    
}


- (void)setUpYUVItem:(YUVPlayItem *)yuv;
{
    YUVItem *item = new YUVItem;
    item->width = yuv.w;
    item->height = yuv.h;
    item->pixelFormat = (AVPixelFormat)yuv.pixelFormat;
    item->fps = yuv.fps;
    item->fileName = [yuv.fileName cStringUsingEncoding:NSUTF8StringEncoding];
    
    self->_core->setYUVItem(item);
    int w = CGRectGetWidth(self.frame);
    int h = CGRectGetHeight(self.frame);
    
    // 计算rect
    int dx = 0;
    int dy = 0;
    int dw = yuv.w;
    int dh = yuv.h;
    
    //计算目标尺寸
    if (dw > w || dh > h) { //缩放
        if ( dw * h > w * dh) { //视频的宽高比 > 播放器的宽高比
            dh = w * dh / dw;
            dw = w;
        } else {
            dw = h * dw / dh;
            dh = h;
        }
    }
    //居中
    dx = (w - dw ) >> 1;
    dy = (h - dh ) >> 1;
    _dstRect = CGRectMake(dx, dy, dw, dh);
    NSLog(@"视频的矩形区域 :%@",NSStringFromRect(_dstRect));
    _imgView.frame = _dstRect;
    [self setNeedsLayout:YES];
}

static int i = 0;
- (void)drawView
{
    NSLog(@"drawView %d",i++);
    int error = 0;
    
    YUVItem *itme = self->_core->getCurrentYUVItem();
    if (!itme) return;
    
    char* buffer = self->_core->getOneFrameRawDataOfRGB24(&error);
    if (!buffer) return;
    
    int width = itme->width;
    int height = itme->height;

    NSImage *img = [self imageWithRGB24:buffer width:width height:height];
    
    if (!img) return;
    
    free(buffer);
    
    [self.imgView setImage:img];
    [self updateLayer];
}

- (NSImage *)imageWithRGB24:(char *)buffer width:(int)width height:(int)height
{
    //转为RGBA32
    //像素总数
    int pixelCount = width * height;
    int pixelTotalBytes = pixelCount * 4;
    char* rgba = (char*)malloc(pixelTotalBytes);
    for(int i=0; i < pixelCount; ++i) {
        rgba[4*i] = buffer[3*i]; //R
        rgba[4*i+1] = buffer[3*i+1];//G
        rgba[4*i+2] = buffer[3*i+2];//B
        rgba[4*i+3] = (char)255; //A 透明度全部改为1
    }
    
    size_t bufferLength = pixelTotalBytes; //宽*高*4(字节) 1个像素占用4个字节
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rgba, bufferLength, NULL);
    size_t bitsPerComponent = 8;//每一个像素的色彩单元(R G B A) 占用的位数 8bit == 1B
    size_t bitsPerPixel = 32;//RGBA = 4*8
    size_t bytesPerRow = 4 * width; //每一行的字节数,一个像素4个字节 一行的像素数==width

    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    if(colorSpaceRef == NULL) {
        NSLog(@"Error allocating color space");
        CGDataProviderRelease(provider);
        if (rgba) {
            free(rgba);
        }
        return nil;
    }

    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;

    CGImageRef iref = CGImageCreate(width,
                                    height,
                                    bitsPerComponent,
                                    bitsPerPixel,
                                    bytesPerRow,
                                    colorSpaceRef,
                                    bitmapInfo,
                                    provider,   // data provider
                                    NULL,       // decode
                                    YES,            // should interpolate
                                    renderingIntent);

    uint32_t* pixels = (uint32_t*)malloc(bufferLength);

    if(pixels == NULL) {
        NSLog(@"Error: Memory not allocated for bitmap");
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpaceRef);
        CGImageRelease(iref);
        if (rgba) {
            free(rgba);
        }
        return nil;
    }
    CGContextRef context = CGBitmapContextCreate(pixels,
                                                 width,
                                                 height,
                                                 bitsPerComponent,
                                                 bytesPerRow,
                                                 colorSpaceRef,
                                                 bitmapInfo);

    NSImage *image = nil;
    if(context) {
        CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, width, height), iref);
        CGImageRef imageRef = CGBitmapContextCreateImage(context);
        image = [[NSImage alloc] initWithCGImage:imageRef size:CGSizeMake(width, height)];
        CGImageRelease(imageRef);
        CGContextRelease(context);
    } else {
        NSLog(@"Error context not created");
        if(pixels) {
            free(pixels);
        }
        if (rgba) {
            free(rgba);
        }
        return nil;
    }

    CGColorSpaceRelease(colorSpaceRef);
    CGImageRelease(iref);
    CGDataProviderRelease(provider);

    if(pixels) {
        free(pixels);
    }
    if (rgba) {
        free(rgba);
    }
    return image;
}

/*
- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    @autoreleasepool {
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self drawView];
        });
    }
    return kCVReturnSuccess;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext)
{
    CVReturn result = [(__bridge YUVMacOSPlayer *)displayLinkContext getFrameForTime:outputTime];
    return result;
}
*/

- (void)play
{
    if (!self->_core->canPlay()) {
        return;
    }
    /* core Video 中的 displayLink
    CGDirectDisplayID   displayID = CGMainDisplayID();
    CVReturn            error = kCVReturnSuccess;
    error = CVDisplayLinkCreateWithCGDisplay(displayID, &displayLink);
    if (error)
    {
        NSLog(@"DisplayLink created with error:%d", error);
        displayLink = NULL;
    }
    CVDisplayLinkSetOutputCallback(displayLink, MyDisplayLinkCallback, (__bridge void *)self);

    CVDisplayLinkStart(displayLink);
     */
    if (!g_timer) {
        dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
         
        dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
         
        // 定时任务调度设置,0秒后启动,每个5秒运行
        dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW ,0);
        /*
         dispatch_source_set_timer(dispatch_source_t source,
             dispatch_time_t start, //开始时间
             uint64_t interval, // 间隔时间
             uint64_t leeway); //可接受误差 纳秒 填0 == 不接受误差
         */
        dispatch_source_set_timer(timer, time, (int)1000/30 * NSEC_PER_MSEC, 0 * NSEC_PER_SEC);
        
        __weak typeof(self) weakSelf = self;
        dispatch_source_set_event_handler(timer, ^{
            // 定时任务
            @autoreleasepool {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf drawView];
                });
            }
        });
         
        dispatch_source_set_cancel_handler(timer, ^{
            // 定时取消回调
            NSLog(@"source did cancel...");
        });
        g_timer = timer;
         
    }
    // 启动定时器
    dispatch_resume(g_timer);
}

@end
