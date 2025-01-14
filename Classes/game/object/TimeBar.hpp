//
//  TimeBar.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#ifndef TimeBar_hpp
#define TimeBar_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../GameManager.hpp"

class TimeBar : public cocos2d::Sprite, public GameListener {
public:
    CREATE_FUNC(TimeBar);
    ~TimeBar();
    
private:
    TimeBar();
    
    bool init() override;
    void onEnter() override;
    void onExit() override;
    
    void update(float dt) override;

    void reset();
    void updateGage();
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGamePause() override;
    void onGameResume() override;
    void onPreGameOver() override;
    void onContinue() override;
    void onGameOver() override;
    
    void onBoostStart() override;
    void onStartTimer() override;
    
public:
    void startTimeBar();
    void stopTimeBar();
    
    void setTimePoint(float point, bool isUpdateGage = true);
    void increaseTimePoint(float point);
    
    float getTimeRatio();
    
private:
    CC_SYNTHESIZE(std::function<void(float, float)>, onTimeChangedListener, OnTimeChangedListener);
    
    GameManager *gameMgr;
    
    SB_SYNTHESIZE_READONLY_BOOL(started, Started);
    float timePoint;
    
    cocos2d::ProgressTimer *gage;
};

#endif /* TimeBar_hpp */
