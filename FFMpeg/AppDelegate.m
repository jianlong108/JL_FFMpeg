//
//  AppDelegate.m
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/19.
//

#import "AppDelegate.h"
#include <libavdevice/avdevice.h>
#include <SDL2/SDL.h>

@interface AppDelegate ()


@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    av_log_set_level(AV_LOG_DEBUG);
    //注册设备 仅执行一次即可
    avdevice_register_all();
    
    SDL_version v;
    SDL_VERSION(&v);
    NSLog(@"SDL_version:%d.%d.%d",v.major,v.minor,v.patch);
//    NSString *urlString = @"x-apple.systempreferences:com.apple.preference.security?Privacy_Assistive";
//        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:urlString]];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
