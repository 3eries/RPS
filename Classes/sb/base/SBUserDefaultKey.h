//
//  SBUserDefaultKey.h
//  RSP
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef SBUserDefaultKey_h
#define SBUserDefaultKey_h

#include "cocos2d.h"

namespace SBUserDefaultKey {
    static const std::string PREFIX                  = "SUPERBOMB_";
    
#define KEY(__key__)    std::string(PREFIX + __key__).c_str()
//#define KEY(__key__)    "SUPERBOMB_"__key__
    
//    static const char* MUTE                          = KEY("MUTE");       // 음소거 여부
    static const char* MUTE                          = "SUPERBOMB_MUTE";    // 음소거 여부
}

#endif /* SBUserDefaultKey_h */
