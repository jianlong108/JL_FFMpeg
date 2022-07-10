//
//  RecordPCM.hpp
//  FFMpeg
//
//  Created by Wangjianlong on 2021/12/21.
//

#ifndef RecordPCM_hpp
#define RecordPCM_hpp

#include <stdio.h>

class JLRecordPCMOrYUV {
public:
    bool stopRecord;
    void beginRecordPCM();
    void beginRecordYUV();
};

#endif /* RecordPCM_hpp */
