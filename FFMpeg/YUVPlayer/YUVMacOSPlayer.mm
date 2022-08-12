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
    CVDisplayLinkRef displayLink;
}

@property (nonatomic, strong) dispatch_source_t timer;
@property (nonatomic, assign) CGRect dstRect;

@property (nonatomic, strong) NSImageView *imgView;
@property (nonatomic, strong) YUVObject *yuv;

@end

@implementation YUVMacOSPlayer

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
    _imgView = [[NSImageView alloc] initWithFrame:self.bounds];
    _imgView.wantsLayer = YES;
    _imgView.layer.backgroundColor = [NSColor redColor].CGColor;
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
    CVDisplayLinkRelease(displayLink);
    
}


- (void)setUpYUVItem:(YUVObject *)yuv;
{
    _yuv = yuv;
    YUVItem item = YUVItem();
    item.width = yuv.w;
    item.height = yuv.h;
    item.pixelFormat = AV_PIX_FMT_YUVA420P;
    item.fps = 30;
    
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
}

static int i = 0;
- (void)drawView
{
    NSLog(@"%d",i++);
    char *buffer = nil;
    int error = 0;
    self->_core->getImageDataFromOneFrame(buffer, &error);
    if (!buffer) return;
    
    int width = self.yuv.w;
    int height = self.yuv.h;
    
    //转为RGBA32
    char* rgba = (char*)malloc(width*height*4);
    for(int i=0; i < width*height; ++i) {
        rgba[4*i] = buffer[3*i];
        rgba[4*i+1] = buffer[3*i+1];
        rgba[4*i+2] = buffer[3*i+2];
        rgba[4*i+3] = 255;
    }
    
    size_t bufferLength = width * height * 4;
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rgba, bufferLength, NULL);
    size_t bitsPerComponent = 8;
    size_t bitsPerPixel = 32;
    size_t bytesPerRow = 4 * width;
    
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    if(colorSpaceRef == NULL) {
        NSLog(@"Error allocating color space");
        CGDataProviderRelease(provider);
        return;
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
        return;
    }
    
    CGContextRef context = CGBitmapContextCreate(pixels,
                                                 width,
                                                 height,
                                                 bitsPerComponent,
                                                 bytesPerRow,
                                                 colorSpaceRef,
                                                 bitmapInfo);
    
    if(context == NULL) {
        NSLog(@"Error context not created");
        free(pixels);
    }
    
    NSImage *image = nil;
    if(context) {
        
        CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, width, height), iref);
        
        CGImageRef imageRef = CGBitmapContextCreateImage(context);
        image = [[NSImage alloc] initWithCGImage:imageRef size:CGSizeMake(width, height)];
//        image = [UIImage imageWithCGImage:imageRef scale:[UIScreen mainScreen].scale orientation:UIImageOrientationUp];
//        if([UIImage respondsToSelector:@selector(imageWithCGImage:scale:orientation:)]) {
//            image = [UIImage imageWithCGImage:imageRef scale:1.0 orientation:UIImageOrientationUp];
//        } else {
//            image = [UIImage imageWithCGImage:imageRef];
//        }
        
        CGImageRelease(imageRef);
        CGContextRelease(context);
    }
    
    CGColorSpaceRelease(colorSpaceRef);
    CGImageRelease(iref);
    CGDataProviderRelease(provider);
    
    if(pixels) {
        free(pixels);
    }
//    return image;
    [self.imgView setImage:image];
}


- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
//    @autoreleasepool {
//        dispatch_sync(dispatch_get_main_queue(), ^{
            [self drawView];
//        });
//    }
    return kCVReturnSuccess;
}

//static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
//                                      const CVTimeStamp* now,
//                                      const CVTimeStamp* outputTime,
//                                      CVOptionFlags flagsIn,
//                                      CVOptionFlags* flagsOut,
//                                      void* displayLinkContext)
//{
//    CVReturn result = [(__bridge YUVMacOSPlayer *)displayLinkContext getFrameForTime:outputTime];
//    return result;
//}

- (void)play
{
    if (!_yuv) {
        return;
    }
//    CGDirectDisplayID   displayID = CGMainDisplayID();
//    CVReturn            error = kCVReturnSuccess;
//    error = CVDisplayLinkCreateWithCGDisplay(displayID, &displayLink);
//    if (error)
//    {
//        NSLog(@"DisplayLink created with error:%d", error);
//        displayLink = NULL;
//    }
//    CVDisplayLinkSetOutputCallback(displayLink, MyDisplayLinkCallback, (__bridge void *)self);
//
//    CVDisplayLinkStart(displayLink);
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
     
    dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
     
    self.timer = timer;
    
    // 定时任务调度设置,0秒后启动,每个5秒运行
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW ,0);
    dispatch_source_set_timer(self.timer, time, 1000/30 * NSEC_PER_MSEC, 3 * NSEC_PER_SEC);
    
    __weak typeof(self) weakSelf = self;
    dispatch_source_set_event_handler(self.timer, ^{
        // 定时任务
        [weakSelf drawView];
    });
     
    dispatch_source_set_cancel_handler(self.timer, ^{
        // 定时取消回调
        NSLog(@"source did cancel...");
    });
     
    // 启动定时器
    dispatch_resume(timer);
}

@end
