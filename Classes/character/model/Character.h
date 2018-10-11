//
//  Character.h
//  RPS
//
//  Created by seongmin hwang on 2018. 10. 11..
//

#ifndef Character_h
#define Character_h

#include "cocos2d.h"
#include "superbomb.h"

enum class UnlockType {
    BEST_SCORE = 1,
    TOTAL_SCORE,
    GAME_OVER,
    DAILY_LOGIN,
    IN_FEVER,
    VIEW_ADS,
};

struct Character {
    std::string     charId;
    std::string     name;
    UnlockType      unlockType;
    int             unlockAmount;
    std::string     unlockDesc;
    
    Character() {
        charId = "";
    }
    
    std::string toString() {
        return STR_FORMAT("Character { charId: %s, name: %s unlockType: %d unlockAmount: %d, unlockDesc: %s }",
                          charId.c_str(), name.c_str(), (int)unlockType, unlockAmount, unlockDesc.c_str());
    }
};

#endif /* Character_h */
