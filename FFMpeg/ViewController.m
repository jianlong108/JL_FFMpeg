//
//  ViewController.m
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/19.
//

#import "ViewController.h"
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavresample/avresample.h>

#import <AVFoundation/AVFoundation.h>

@interface ViewController ()

@property(nonatomic, strong) NSSwitch *beiginRecordSwitch;
@property(nonatomic, assign) BOOL stop;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSLog(@"%@",[[NSString alloc] initWithCString:av_version_info() encoding:NSUTF8StringEncoding]);
    
    NSButton *beiginRecordBtn = [NSButton buttonWithTitle:@"开始录音" target:self action:@selector(beginRecord:)];
    beiginRecordBtn.frame = CGRectMake(0, 0, 100, 44);
    [self.view addSubview:beiginRecordBtn];
    
    
    NSButton *endRecordBtn = [NSButton buttonWithTitle:@"停止录音" target:self action:@selector(endRecord:)];
    endRecordBtn.frame = CGRectMake(0, 100, 100, 44);
    [self.view addSubview:endRecordBtn];
    
    _beiginRecordSwitch = [[NSSwitch alloc] init];
    _beiginRecordSwitch.frame = CGRectMake(150, 0, 100, 44);
    [self.view addSubview:_beiginRecordSwitch];
    
    
    _beiginRecordSwitch.state = NSControlStateValueOn;
    
    
    NSString *baseFilePath = [NSHomeDirectory() stringByAppendingPathComponent:@"Desktop"];
    NSString *filePath = [baseFilePath stringByAppendingPathComponent:@"out_code.txt"];
    BOOL success = [[NSFileManager defaultManager] createFileAtPath:filePath contents:nil attributes:nil];

    NSFileHandle *fileHandle = nil;
    if (success) {
        fileHandle = [NSFileHandle fileHandleForWritingAtPath:filePath];
    }
    
    for (int i = 0; i<99; i++) {
        unsigned long long offset = 0;
        NSError *error;
        [fileHandle seekToEndReturningOffset:&offset error:&error];
        if (!error) {
            NSError *writeError = nil;
            [fileHandle writeData:[@(i).stringValue dataUsingEncoding:NSUTF8StringEncoding] error:&writeError];
            if (writeError) {
                NSLog(@"%@",writeError);
            }
        } else {
            NSLog(@"%@",error);
            ;
        }
    }
    
//    [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
//
//    }];
//    [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
}

- (void)endRecord:(id)sender
{
    if (_stop ==  NO) {
        _stop = YES;
    }
}
void showSpec(AVFormatContext *ctx) {
    // 获取输入流
    AVStream *stream = ctx->streams[0];
    // 获取音频参数
    AVCodecParameters *params = stream->codecpar;
    // 声道数
    NSLog(@"%d",params->channels);
    // 采样率
    
    NSLog(@"%d",params->sample_rate);
    // 采样格式
    NSLog(@"%d",params->format);
    // 每一个样本的一个声道占用多少个字节
//    av_get_bytes_per_sample((AVSampleFormat) params->format);
//    NSLog(@"%d",);
}
- (void)beginRecord:(id)sender
{
    _beiginRecordSwitch.state = NSControlStateValueOff;
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        AVInputFormat *inFmt = av_find_input_format("avfoundation");
        if (!inFmt) {
            NSLog(@"找不到输入格式: avfoundation");
            return;
        }
        AVFormatContext *ctx = NULL;//avformat_alloc_context();
        //int avformat_open_input(AVFormatContext **ps, const char *url, ff_const59 AVInputFormat *fmt, AVDictionary **options);
    //    AVDictionary *options = nil;
    //    av_dict_set(&options, "framerate", "30", 0);
    //    av_dict_set(&options, "video_size", "1280x720", 0);
        int ret = avformat_open_input(&ctx,":1",inFmt,NULL);
        if (ret < 0) {
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof (errbuf));
            printf("avformat_open_input error: %s \n",errbuf);
            return;
        }
        showSpec(ctx);
    //    "~/Desktop/out.pcm"
        NSString *baseFilePath = [NSHomeDirectory() stringByAppendingPathComponent:@"Desktop"];
        NSString *filePath = [baseFilePath stringByAppendingPathComponent:@"out_code.pcm"];
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
            [[NSFileManager defaultManager] removeItemAtPath:filePath error:NULL];
        }
        BOOL success = [[NSFileManager defaultManager] createFileAtPath:filePath contents:nil attributes:nil];

        NSFileHandle *fileHandle = nil;
        if (success) {
            fileHandle = [NSFileHandle fileHandleForWritingAtPath:filePath];
        }
        if (!success) {
            
            NSLog(@"打开文件失败 ~/Desktop/out_code.pcm");
            
            // 关闭设备
            avformat_close_input(&ctx);
            return;
        }
        

        // 数据包
        AVPacket *pkt = av_packet_alloc();
        pkt->data = NULL;
        pkt->size = 0;
        
//        FILE *output_fd = fopen([filePath cStringUsingEncoding:NSUTF8StringEncoding], "wb+");
//        assert(output_fd);
//
//        while (!_stop) {
//            ret = av_read_frame(ctx, pkt);
//            if (ret < 0) {
//                if (ret == -35) {
//                    continue;
//                }
//
//                break;
//            }
//            fwrite(pkt->data, pkt->size, 1, output_fd);
//            fflush(output_fd);
//            av_packet_unref(pkt);
//        }
//
//        av_packet_free(&pkt);
//        avformat_close_input(&ctx);
       
        while (!_stop) {
            int ret = av_read_frame(ctx, pkt);

            if (ret == 0) {
                // 将数据写入文件
                unsigned long long offset = 0;
                NSError *error;
                [fileHandle seekToEndReturningOffset:&offset error:&error];
                if (!error) {
                    NSError *writeError = nil;
                    [fileHandle writeData:[NSData dataWithBytes:pkt->data length:pkt->size] error:&writeError];
                    if (writeError) {
                        NSLog(@"writeData:error:%@",writeError);
                    }
                } else {
                    NSLog(@"seekToEndReturningOffset %@",error);
                    continue;
                }
                // 释放资源
                av_packet_unref(pkt);
            } else if (ret == AVERROR(EAGAIN)) { // 资源临时不可用
                NSLog(@"资源临时不可用");
                continue;
            } else { // 其他错误
                char errbuf[1024];
                av_strerror(ret, errbuf, sizeof (errbuf));

                printf("av_read_frame error code %d %s \n",ret,errbuf);
                break;
            }
        }
        NSError *closeError = nil;
        [fileHandle closeAndReturnError:&closeError];
        if (closeError) {
            NSLog(@"closeAndReturnError :%@",closeError);
        }

        // 释放资源
        av_packet_free(&pkt);
        // 关闭设备
        avformat_close_input(&ctx);
        NSLog(@"----结束-----");
        dispatch_async(dispatch_get_main_queue(), ^{
            self.beiginRecordSwitch.state = NSControlStateValueOn;
        });
    });
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
