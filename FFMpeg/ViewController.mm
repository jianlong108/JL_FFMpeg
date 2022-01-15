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
#import "JLPlayWAV.hpp"
#import "JLAudioResample.hpp"


@interface ViewController ()
{
    JLRecordPCM *recordObj;
    JLPlayPCM *playPCMObj;
    JLPlayWAV *playWAVObj;
}

@property(nonatomic, strong) NSSwitch *beiginRecordSwitch;
@property(nonatomic, assign) BOOL stopRecord;
@property(nonatomic, assign) BOOL stopPlay;



@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSButton *beiginRecordBtn = [NSButton buttonWithTitle:@"开始录音pcm" target:self action:@selector(beginRecord:)];
    beiginRecordBtn.frame = CGRectMake(0, 0, 188, 44);
    [self.view addSubview:beiginRecordBtn];
    
    
    NSButton *endRecordBtn = [NSButton buttonWithTitle:@"停止录音pcm" target:self action:@selector(endRecord:)];
    endRecordBtn.frame = CGRectMake(200, 0, 188, 44);
    [self.view addSubview:endRecordBtn];
    
    NSButton *beiginPlayBtn = [NSButton buttonWithTitle:@"开始播放pcm" target:self action:@selector(beginPlay:)];
    beiginPlayBtn.frame = CGRectMake(0, 50, 188, 44);
    [self.view addSubview:beiginPlayBtn];
    
    
    NSButton *endPlayBtn = [NSButton buttonWithTitle:@"停止播放pcm" target:self action:@selector(endPlay:)];
    endPlayBtn.frame = CGRectMake(200, 50, 188, 44);
    [self.view addSubview:endPlayBtn];
    
    
    NSButton *beiginPlayWAVBtn = [NSButton buttonWithTitle:@"开始播放wav" target:self action:@selector(beginPlayWAV:)];
    beiginPlayWAVBtn.frame = CGRectMake(0, 150, 188, 44);
    [self.view addSubview:beiginPlayWAVBtn];
    
    
    NSButton *endPlayWAVBtn = [NSButton buttonWithTitle:@"停止播放wav" target:self action:@selector(endPlayWAV:)];
    endPlayWAVBtn.frame = CGRectMake(200, 150, 188, 44);
    [self.view addSubview:endPlayWAVBtn];
    
    
    _beiginRecordSwitch = [[NSSwitch alloc] init];
    _beiginRecordSwitch.frame = CGRectMake(400, 0, 44, 44);
    [self.view addSubview:_beiginRecordSwitch];
    
    
    _beiginRecordSwitch.state = NSControlStateValueOn;
    
    NSButton *pcmToWAVBtn = [NSButton buttonWithTitle:@"开始转换pcm2wav" target:self action:@selector(beginConvert:)];
    pcmToWAVBtn.frame = CGRectMake(0, 100, 188, 44);
    [self.view addSubview:pcmToWAVBtn];
    
    
    NSButton *audioResampleBtn = [NSButton buttonWithTitle:@"音频重采样" target:self action:@selector(audioResample:)];
    audioResampleBtn.frame = CGRectMake(200, 100, 188, 44);
    [self.view addSubview:audioResampleBtn];
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
        self->playPCMObj = new JLPlayPCM;
        self->playPCMObj->beginPlay();
    });
}

- (void)endPlay:(id)sender
{
    self->playPCMObj->stopPlay = true;
}


- (void)beginConvert:(id)sender
{
    // 封装WAV的头部
    WAVHeader header;
    header.numChannels = 2;
    header.sampleRate = 44100;
    header.bitsPerSample = 32;
    JLPCMToWAV::pcm2wav(header, "/Users/dalong/Desktop/AV/3.pcm", "/Users/dalong/Desktop/AV/7.wav");
}

- (void)beginPlayWAV:(id)sender
{
    
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        self->playWAVObj = new JLPlayWAV;
        self->playWAVObj->beginPlay();
    });
}

- (void)endPlayWAV:(id)sender
{
    self->playWAVObj->stopPlay = true;
}

- (void)audioResample:(id)sender
{
    JLAudioResample::resampleAudio("/Users/dalong/Desktop/AV/44100_s16le_2.pcm", 44100, AV_SAMPLE_FMT_S16, 2, "/Users/dalong/Desktop/AV/48000_f32le_2.pcm", 48000, AV_SAMPLE_FMT_FLT, 2);
}

@end
