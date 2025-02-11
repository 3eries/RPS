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

#include "BaseScene.hpp"
#include "Define.h"

#include "GameManager.hpp"

class GameView;

class GameScene : public BaseScene, public GameListener {
public:
    static GameScene* create(GiftRewardItem boostItem = GiftRewardItem());
    ~GameScene();
    
private:
    enum Tag {
        BANNER_LOADING           = 100,
        
        POPUP_PAUSE              = 10000,
        POPUP_CONTINUE,
        POPUP_GAME_OVER,
    };
    
private:
    GameScene();
    
    bool init(GiftRewardItem boostItem);
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    bool onBackKeyReleased() override;
    
    void initBg();
    void initCommonMenu() override;
    void initBanner();
    
    void initGiftListener();
    
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
    void showGameOverPopup(OnPopupEvent onEventListener = nullptr);

    void checkReview();
    
    void onClick(cocos2d::Node *sender) override;
    bool onClickTopMenu(TopMenu::Tag tag);
    bool onClickBottomMenu(BottomMenu::Tag tag);
    
private:
    GameManager *gameMgr;
    CC_SYNTHESIZE(GameView*, gameView, GameView);
    
    cocos2d::Node *touchLockNode;   // 터치 방지 노드
};

#endif /* GameScene_hpp */
