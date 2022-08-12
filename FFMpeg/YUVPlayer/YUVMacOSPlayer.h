//
//  YUV420PPlayer.h
//  FFMpeg
//
//  Created by JL on 2022/8/8.
//

#import <Cocoa/Cocoa.h>
#include "YUVObject.h"
//#include "YUVItem.hpp"

NS_ASSUME_NONNULL_BEGIN

@interface YUVMacOSPlayer : NSView

- (void)setUpYUVItem:(YUVObject *)yuv;
- (void)play;

@end

NS_ASSUME_NONNULL_END
