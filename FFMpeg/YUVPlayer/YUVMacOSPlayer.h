//
//  YUV420PPlayer.h
//  FFMpeg
//
//  Created by JL on 2022/8/8.
//

#import <Cocoa/Cocoa.h>
#include "YUVPlayItem.h"

NS_ASSUME_NONNULL_BEGIN

@interface YUVMacOSPlayer : NSView

- (void)setUpYUVItem:(YUVPlayItem *)yuv;
- (void)play;

@end

NS_ASSUME_NONNULL_END
