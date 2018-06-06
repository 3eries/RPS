//
//  GameConfiguration.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#ifndef GameConfiguration_hpp
#define GameConfiguration_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "RSP.h"

struct TimeInfo {
    int maxPoint;               // 최대 포인트
    int firstPoint;             // 최초 포인트
    int increasePointPerHit;    // 블럭 히트 획득 포인트
    float drawDelay;            // 비긴 경우의 터치 지연 시간
    
    TimeInfo() {
    }
};

struct FeverInfo {
    float maxPoint;             // 피버 진입 포인트
    std::map<RSPType, float> points; // 타입 별 획득 포인트
    float gageDuration;         // 게이지 지속 시간
    float duration;             // 피버 지속 시간
    float endDelay;             // 피버 종료 유예 시간
    
    FeverInfo() {
    }
};

struct LevelInfo {
    int level;
    int beginRange; // 레벨 시작 범위 (스코어)
    int decreasePointPerSeconds; // 1초당 감소 포인트
    
    LevelInfo() {
        level = -1;
    }
};

class GameConfiguration : public cocos2d::Ref {
public:
    static GameConfiguration* getInstance();
    static void destroyInstance();
    ~GameConfiguration();
    
private:
    GameConfiguration();
    
public:
    void init();
    
    LevelInfo getLevelInfo(int level);
    LevelInfo getMaxLevelInfo();
    
private:
    CC_SYNTHESIZE(TimeInfo, timeInfo, TimeInfo);
    CC_SYNTHESIZE(FeverInfo, feverInfo, FeverInfo);
    CC_SYNTHESIZE(std::vector<LevelInfo>, levelInfos, LevelInfos);
};

#endif /* GameConfiguration_hpp */
