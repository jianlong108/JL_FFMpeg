//
//  FFMpegs.hpp
//  FFMpeg
//
//  Created by JL on 2022/8/12.
//

#ifndef FFMpegs_hpp
#define FFMpegs_hpp

#include <stdint.h>

extern "C" {
#include <libavutil/avutil.h>
}

typedef struct {
    char *pixels;
    int width;
    int height;
    AVPixelFormat format;
} RawVideoFrame;

typedef struct {
    const char *filename;
    int width;
    int height;
    AVPixelFormat format;
} RawVideoFile;

class FFmpegTool {
public:
    FFmpegTool();
    static void convertRawVideo(RawVideoFrame &in,
                                RawVideoFrame &out);
    static void convertRawVideo(RawVideoFile &in,
                                RawVideoFile &out);
};


#endif /* FFMpegs_hpp */
