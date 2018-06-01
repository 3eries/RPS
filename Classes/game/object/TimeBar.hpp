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

    void updateGage();
    
// GameListener
private:
    void onGameStart() override;
    void onGameOver() override;
    
public:
    void increase(float dt);
    
private:
    float duration;
    float elapsed;
    
    cocos2d::ProgressTimer *gage;
};

#endif /* TimeBar_hpp */
