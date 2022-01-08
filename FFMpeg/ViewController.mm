//
//  ViewController.m
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/19.
//

#import "ViewController.h"

#import "JLRecordPCM.hpp"
#import "JLPlayPCM.hpp"
#import "JLPCMToWAV.hpp"

//#import <AVFoundation/AVFoundation.h>


@interface ViewController ()
{
    JLRecordPCM *recordObj;
    JLPlayPCM *playObj;
}

@property(nonatomic, strong) NSSwitch *beiginRecordSwitch;
@property(nonatomic, assign) BOOL stopRecord;
@property(nonatomic, assign) BOOL stopPlay;



@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSButton *beiginRecordBtn = [NSButton buttonWithTitle:@"开始录音" target:self action:@selector(beginRecord:)];
    beiginRecordBtn.frame = CGRectMake(0, 0, 88, 44);
    [self.view addSubview:beiginRecordBtn];
    
    
    NSButton *endRecordBtn = [NSButton buttonWithTitle:@"停止录音" target:self action:@selector(endRecord:)];
    endRecordBtn.frame = CGRectMake(100, 0, 88, 44);
    [self.view addSubview:endRecordBtn];
    
    NSButton *beiginPlayBtn = [NSButton buttonWithTitle:@"开始播放" target:self action:@selector(beginPlay:)];
    beiginPlayBtn.frame = CGRectMake(0, 50, 88, 44);
    [self.view addSubview:beiginPlayBtn];
    
    
    NSButton *endPlayBtn = [NSButton buttonWithTitle:@"停止播放" target:self action:@selector(endPlay:)];
    endPlayBtn.frame = CGRectMake(100, 50, 88, 44);
    [self.view addSubview:endPlayBtn];
    
    
    _beiginRecordSwitch = [[NSSwitch alloc] init];
    _beiginRecordSwitch.frame = CGRectMake(200, 0, 88, 44);
    [self.view addSubview:_beiginRecordSwitch];
    
    
    _beiginRecordSwitch.state = NSControlStateValueOn;
    
    NSButton *pcmToWAVBtn = [NSButton buttonWithTitle:@"开始转换" target:self action:@selector(beginConvert:)];
    pcmToWAVBtn.frame = CGRectMake(0, 100, 88, 44);
    [self.view addSubview:pcmToWAVBtn];
    
    
}

- (void)endRecord:(id)sender
{
    if (recordObj->stopRecord ==  false) {
        recordObj->stopRecord = true;
        self.beiginRecordSwitch.state = NSControlStateValueOn;
    }
}
- (void)beginRecord:(id)sender
{
    _beiginRecordSwitch.state = NSControlStateValueOff;
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        self->recordObj = new JLRecordPCM;
        self->recordObj->beginRecord();
        return;
    });
}

- (void)beginPlay:(id)sender
{
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        self->playObj = new JLPlayPCM;
        self->playObj->beginPlay();
    });
}
- (void)endPlay:(id)sender{}


- (void)beginConvert:(id)sender
{
    // 封装WAV的头部
    WAVHeader header;
    header.numChannels = 2;
    header.sampleRate = 44100;
    header.bitsPerSample = 32;
    JLPCMToWAV::pcm2wav(header, "/Users/dalong/Desktop/AV/3.pcm", "/Users/dalong/Desktop/AV/3.wav");
}

@end
