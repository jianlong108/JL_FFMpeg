//
//  JLAACACodec.hpp
//  FFMpeg
//
//  Created by Wangjianlong on 2022/1/15.
//

#ifndef JLAACACodec_hpp
#define JLAACACodec_hpp

#include <stdio.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
}

// 错误处理
#define ERROR_BUF(ret) \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));


// 参数
typedef struct {
    const char *filename;
    int sampleRate;
    AVSampleFormat sampleFmt;
    //AV_CH_LAYOUT_STEREO 立体声
    int chLayout;
} AudioEncodeSpec;

class JLAACCodec {
public:
    JLAACCodec();
    
    // 编码
    static void aacEncode(AudioEncodeSpec &in,
                          const char *outFilename);
    
    // 解码
    static void aacDecode(const char *inFilename,
                          AudioEncodeSpec &out);

};

#endif /* JLAACACodec_hpp */
