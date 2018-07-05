//
//  SBUserDefaultKey.h
//  RSP
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef SBUserDefaultKey_h
#define SBUserDefaultKey_h

#include "cocos2d.h"

#define SB_USER_DEFAULT_KEY(__key__)    std::string(PREFIX + __key__).c_str()
//#define KEY(__key__)    "SUPERBOMB_"__key__

namespace SBUserDefaultKey {
    static const std::string PREFIX                  = "SUPERBOMB_";
    
    // static const char* EFFECT_MUTE                   = SB_USER_DEFAULT_KEY("EFFECT_MUTE");
    static const char* EFFECT_MUTE                   = "SUPERBOMB_EFFECT_MUTE";    // 효과음 음소거 여부
    static const char* BGM_MUTE                      = "SUPERBOMB_BGM_MUTE";       // 배경음 음소거 여부
}

#endif /* SBUserDefaultKey_h */
