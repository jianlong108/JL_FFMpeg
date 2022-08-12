//
//  YUVItem.hpp
//  FFMpeg
//
//  Created by JL on 2022/8/11.
//

#import <Foundation/Foundation.h>
//extern "C" {
//#include <libavutil/avutil.h>
//}
//
//class YUVItem {
//    
//    
//public:
//    int width;
//    int height;
//    const char *fileName;
//    AVPixelFormat pixelFormat;
//    char *filePath = nullptr;
//    
//    YUVItem(int width, int height, AVPixelFormat pixelFormat, const char *fileName);
//    ~YUVItem();
//};

@interface YUVObject : NSObject

@property (nonatomic,assign) int w;
@property (nonatomic,assign) int h;
@property (nonatomic,copy) NSString *fileName;
@property (nonatomic,copy) NSString *filePath;
@property (nonatomic,assign) int pixelFormat;

@end
//#endif /* YUVItem_hpp */
