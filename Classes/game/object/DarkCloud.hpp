//
//  DarkCloud.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#ifndef DarkCloud_hpp
#define DarkCloud_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../GameManager.hpp"

class TimeBar;

/** @class DarkCloud
 * @brief 먹구름
 */
class DarkCloud : public cocos2d::Node, public GameListener {
public:
    CREATE_FUNC(DarkCloud);
    ~DarkCloud();
    
private:
    DarkCloud();
    
    bool init() override;
    void onExit() override;
    
    void reset();
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGamePause() override;
    void onGameResume() override;
    void onPreGameOver() override;
    void onContinue() override;
    void onGameOver() override;
    void onStartTimer() override;
    void onLevelChanged(LevelInfo level) override;
    
public:
    void update(float dt) override;
    
    void updateCloud(float timeRatio, bool forceUpdate = false);
    void updateCloudPosition(float timeRatio, bool forceMove = false);
    void updateCloudOpacity(float timeRatio);
    
    float getCloudPosition(float timeRatio);
    float getCloudOpacity(float timeRatio);
    
private:
    GameManager *gameMgr;
    CC_SYNTHESIZE(TimeBar*, timeBar, TimeBar);
    
    float timeRatio;
    bool isPositionLocked;      // 좌표 변경 잠금 여부
    bool isOpacityLocked;       // 투명도 변경 잠금 여부
    
    float targetPosition;       // 목표 좌표
    
    spine::SkeletonAnimation *anim;
};

#endif /* DarkCloud_hpp */
