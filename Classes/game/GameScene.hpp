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
    typedef enum {
        BTN_PAUSE                = 100,
        
        POPUP_PAUSE              = 1000,
        POPUP_GAME_OVER,
        
    } Tag;
    
private:
    GameScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initGameView();
    void initMenu();
    
    void onClick(cocos2d::Node *sender) override;
    
// GameListener
private:
    void onGameStart() override;
    void onGameOver() override;
    
private:
    void replaceMainScene();
    
    void showPausePopup();
    void removePausePopup();
    
    void showGameOver();
    
private:
    GameManager *gameMgr;
    
    GameView *gameView;
    cocos2d::Node *menuLayer;
};

#endif /* GameScene_hpp */
