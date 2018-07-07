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
class MainMenu;

class GameScene : public cocos2d::Scene, public SBNodeListener, public GameListener {
public:
    CREATE_FUNC(GameScene);
    ~GameScene();
    
private:
    enum Tag {
        LAYER_MENU               = 100,
        
        BTN_PAUSE                = 1000,
        
        POPUP_PAUSE              = 10000,
        POPUP_CONTINUE,
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
    
public:
    void replaceMain();
    
    void showPausePopup();
    void showContinuePopup();
    void showNewRecordPopup(int ranking, int score);
    void showGameOver();

    void onClick(cocos2d::Node *sender) override;
    
private:
    GameManager *gameMgr;
    CC_SYNTHESIZE(GameView*, gameView, GameView);
    
    MainMenu *mainMenu;
    
    cocos2d::Node *touchLockNode;   // 터치 방지 노드
    cocos2d::Sprite *banner; // 임시
};

#endif /* GameScene_hpp */
