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
    NONE = 0,
    BEST_SCORE,     // 최고 스코어
    TOTAL_SCORE,    // 달성한 스코어의 총 합
    GAME_PLAY,      // 플레이한 게임의 판 수
    GAME_OVER,      // 게임 오버한 횟수
    DAILY_LOGIN,    // 게임에 접속한 횟수
    FEVER,          // 피버 진입 횟수
    VIEW_ADS,       // 동영상 광고 시청 횟수
};

struct Character {
    std::string              charId;
    std::string              name;
    UnlockType               unlockType;
    int                      unlockAmount;
    std::string              unlockDesc;
    std::vector<std::string> idleAnims;
    std::vector<std::string> attackAnims;
    cocos2d::Vec2            feverGagePos;
    
    Character() {
        charId = "";
    }
    
    std::string toString() {
        std::string str = "\tCharacter {\n";
        str += STR_FORMAT("\t\tcharId: %s, name: %s unlockType: %d unlockAmount: %d, unlockDesc: %s",
                          charId.c_str(), name.c_str(), (int)unlockType, unlockAmount, unlockDesc.c_str());
        str += "\n\t\tidleAnims: ";
        
        for( std::string anim : idleAnims ) {
            str += anim + ", ";
        }

        str += "\n\t\tattackAnims: ";
        
        for( std::string anim : attackAnims ) {
            str += anim + ", ";
        }
        
        str += "\n\t}";
        
        return str;
    }
};

#endif /* Character_h */
