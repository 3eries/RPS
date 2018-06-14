//
//  GameScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"
#include "RSP.h"

#include "GameManager.hpp"

class GameView;

class GameScene : public cocos2d::Scene, public SBNodeListener, public GameListener {
public:
    CREATE_FUNC(GameScene);
    ~GameScene();
    
private:
    enum  Tag{
        BTN_PAUSE                = 100,
        
        POPUP_PAUSE              = 1000,
        POPUP_GAME_OVER,
    };
    
private:
    GameScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initMenu();
    
// GameListener
private:
    void onGameStart() override;
    void onGameOver() override;
    
private:
    void replaceMain();
    
    void showPausePopup();
    void removePausePopup();
    
    void showGameOver();

    void onClick(cocos2d::Node *sender) override;
    
private:
    GameManager *gameMgr;
    CC_SYNTHESIZE(GameView*, gameView, GameView);
    
    cocos2d::Sprite *banner; // 임시
};

#endif /* GameScene_hpp */
