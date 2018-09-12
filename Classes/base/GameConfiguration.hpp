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
    float maxPoint;               // 최대 포인트
    float firstPoint;             // 최초 포인트
    float continuePoint;          // 이어하기 후 최초 포인트
    float increasePointPerHit;    // 블럭 히트 획득 포인트
    float drawDelay;              // 비긴 경우의 터치 지연 시간
    
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
    int beginRange;         // 레벨 시작 범위 (스코어)
    float increasePointPerHit;      // 블럭 히트 획득 포인트
    float increasePointPerFeverHit; // 피버 블럭 히트 획득 포인트
    float decreasePointPerSeconds;  // 1초당 감소 포인트
    float feverGageRate;    // 피버 게이지 가위바위보 획득 포인트의 가중치
    float gageDuration;     // 게이지 지속 시간
    int continuation;       // 연속으로 같은 블럭이 나올 확률 (0~100)
    
    LevelInfo() {
        level = -1;
        increasePointPerHit = 0;
        increasePointPerFeverHit = 0;
        decreasePointPerSeconds = 0;
        feverGageRate = 1;
        gageDuration = 0;
        continuation = 0;
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
    void parse(const std::string &json);
    
    LevelInfo getLevelInfo(int level);
    LevelInfo getMaxLevelInfo();
    
private:
    CC_SYNTHESIZE_READONLY(std::string, storeUrl, StoreUrl);
    
    // 최대 스코어 제한
    CC_SYNTHESIZE_READONLY(int, maxScore, MaxScore);
    
    // 이어하기 스코어 조건
    CC_SYNTHESIZE_READONLY(int, continueCondition, ContinueCondition);
    
    int continuation;
    CC_SYNTHESIZE(TimeInfo, timeInfo, TimeInfo);
    CC_SYNTHESIZE(FeverInfo, feverInfo, FeverInfo);
    CC_SYNTHESIZE(std::vector<LevelInfo>, levelInfos, LevelInfos);
};

#endif /* GameConfiguration_hpp */
