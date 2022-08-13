//
//  YUVPlayItem.h
//  FFMpeg
//
//  Created by JL on 2022/8/11.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSUInteger, YUVPlayItemPixFormate) {
    YUVPlayItemPixFormateYUV420P                = 0,
};

@interface YUVPlayItem : NSObject

@property (nonatomic,assign) int w;
@property (nonatomic,assign) int h;
@property (nonatomic,assign) int fps;
@property (nonatomic,copy) NSString *fileName;
@property (nonatomic,copy) NSString *filePath;
@property (nonatomic,assign) YUVPlayItemPixFormate pixelFormat;

@end
