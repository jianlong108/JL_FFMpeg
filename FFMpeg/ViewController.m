//
//  ViewController.m
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/19.
//

#import "ViewController.h"
#include <libavutil/avutil.h>

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"%@",[[NSString alloc] initWithCString:av_version_info() encoding:NSUTF8StringEncoding]);
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
