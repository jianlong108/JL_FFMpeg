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
    beiginRecordBtn.frame = CGRectMake(0, 0, 100, 44);
    [self.view addSubview:beiginRecordBtn];
    
    
    NSButton *endRecordBtn = [NSButton buttonWithTitle:@"停止录音" target:self action:@selector(endRecord:)];
    endRecordBtn.frame = CGRectMake(0, 100, 100, 44);
    [self.view addSubview:endRecordBtn];
    
    NSButton *beiginPlayBtn = [NSButton buttonWithTitle:@"开始播放" target:self action:@selector(beginPlay:)];
    beiginPlayBtn.frame = CGRectMake(0, 200, 100, 44);
    [self.view addSubview:beiginPlayBtn];
    
    
    NSButton *endPlayBtn = [NSButton buttonWithTitle:@"停止播放" target:self action:@selector(endPlay:)];
    endPlayBtn.frame = CGRectMake(0, 300, 100, 44);
    [self.view addSubview:endPlayBtn];
    
    
    _beiginRecordSwitch = [[NSSwitch alloc] init];
    _beiginRecordSwitch.frame = CGRectMake(150, 0, 100, 44);
    [self.view addSubview:_beiginRecordSwitch];
    
    
    _beiginRecordSwitch.state = NSControlStateValueOn;
    
    // 封装WAV的头部
    WAVHeader header;
    header.numChannels = 2;
    header.sampleRate = 44100;
    header.bitsPerSample = 32;
    JLPCMToWAV::pcm2wav(header, "/Users/dalong/Desktop/jl_2021-12-21_23-10-37.pcm", "/Users/dalong/Desktop/jl_2021-12-21_23-10-37.wav");
    
//    [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
//
//    }];
//    [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
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


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
