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

#include "../GameManager.hpp"

class Man : public cocos2d::Node, public GameListener {
public:
    enum class Position {
        LEFT,
        RIGHT,
    };
    
    enum class AnimationType {
        IDLE,
        ATTACK,
    };
    
public:
    CREATE_FUNC(Man);
    ~Man();
    
private:
    Man();
    
    bool init() override;
    void onExit() override;
    
    void update(float dt) override;
    
    void initImage();
    void initFeverGage();
    
    void setManAnimation(AnimationType animType, bool runAnimation = true);
    void setManPosition(Position pos);
    
    void setFeverPoint(float point, bool isUpdateGage = true);
    void addFeverPoint(float point, bool isUpdateGage = true);
    void resetFeverPoint(bool isRunAction = true);
    void updateFeverGage();
    
    void runFeverMode();
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGameOver() override;
    void onGamePause() override;
    void onGameResume() override;
    void onGameModeChanged(GameMode mode) override;
    
public:
    void showdown(RSPResult result, RSPType myHand, RSPType oppHand);
    void rockNroll(Position pos);
    
    void resultWin(RSPType myHand, RSPType oppHand);
    void resultLose(RSPType myHand, RSPType oppHand);
    void resultDraw(RSPType myHand, RSPType oppHand);
    
private:
    GameManager *gameMgr;
    
    CC_SYNTHESIZE(RSPType, lastWinHand, LastWinHand);
    double lastShowdownTime; // 단위 : sec
    
    CC_SYNTHESIZE_READONLY(Position, manPosition, ManPosition);
    
    SBAnimationSprite *img;
    
    struct FeverGage {
        float point;
        cocos2d::Node *bg;
//        cocos2d::ProgressTimer *gage;
        cocos2d::Sprite *gage;
        
        FeverGage() {
            point = 0;
        }
    };
    
    FeverGage feverGage;
};

#endif /* Man_hpp */
