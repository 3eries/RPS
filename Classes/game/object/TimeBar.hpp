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
    void onExit() override;
    
    void update(float dt) override;

    void reset();
    void updateGage();
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGameOver() override;
    void onGamePause() override;
    void onGameResume() override;
    
public:
    void setTimePoint(int point);
    void increaseTimePoint(int point);
    
private:
    GameManager *gameMgr;
    int timePoint;
    
    cocos2d::ProgressTimer *gage;
};

#endif /* TimeBar_hpp */
