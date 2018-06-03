//
//  Man.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#ifndef Man_hpp
#define Man_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "RSP.h"

#include "../../base/ViewManager.hpp"
#include "../GameManager.hpp"

class Man : public cocos2d::Node, public ViewListener, public GameListener {
public:
    enum class Position {
        LEFT,
        RIGHT,
    };
    
public:
    CREATE_FUNC(Man);
    ~Man();
    
private:
    Man();
    
    bool init() override;
    void onExit() override;
    
    void update(float dt) override;
    
    void onViewChanged(ViewType viewType) override;
    
    void initImage();
    void initFeverGage();
    
    void setManPosition(Position pos);
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGameOver() override;
    void onGamePause() override;
    void onGameResume() override;
    
public:
    void showdown(/*RSPType type*/);
    void hit(RSPType type);
    
private:
    CC_SYNTHESIZE(RSPType, lastHitType, LastHitType);
    CC_SYNTHESIZE_READONLY(Position, manPosition, ManPosition);
    
private:
    SBAnimationSprite *img;
    
    struct FeverGage {
        cocos2d::Node *bg;
        cocos2d::ProgressTimer *gage;
        // cocos2d::Sprite *gage;
    };
    
    FeverGage feverGage;
};

#endif /* Man_hpp */
