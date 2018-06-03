//
//  GameView.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameView_hpp
#define GameView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RSP.h"
#include "GameManager.hpp"

#include "../base/ViewManager.hpp"

class RSPBlock;
class RSPButton;
class Man;
class TimeBar;

class GameView : public cocos2d::Node, public SBNodeListener, public ViewListener, public GameListener {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    typedef enum {
        LAYER_MENU               = 10,
        
        BTN_PAUSE                = 100,
        
        POPUP_PAUSE              = 1000,
        POPUP_GAME_OVER,
        
    } Tag;
    
private:
    GameView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBlocks();
    void initButtons();
    void initMan();
    void initTimeBar();
    void initLabels();
    void initMenu();
    
    void onViewChanged(ViewType viewType) override;
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGameOver() override;
    
private:
    void onClickButton(RSPType type);
    
    void hitBlock(RSPBlock *block, RSPType btnType);
    void misBlock(RSPBlock *block);
    void drawBlock(RSPBlock *block);
    
    void runHitBlockEffect(RSPBlock *hitBlock);
    
    void alignBlock(int i, RSPBlock *block);
    cocos2d::Vec2 getBlockPosition(int i);
    
private:
    void showPausePopup();
    void removePausePopup();
    
    void showGameOver();
    
    void replaceMain();
    
    void onClick(cocos2d::Node *sender) override;
    
private:
    GameManager *gameMgr;
    std::vector<cocos2d::Node*> gameNodes;  // 게임 뷰타입의 노드 리스트
    
    // 블럭
    cocos2d::Node *blockLayer;
    std::vector<RSPBlock*> blocks;
    int blockIndex;
    
    // 가위바위보 버튼
    std::vector<RSPButton*> rspButtons;
    
    // 캐릭터
    Man *man;
    
    // 시간 제한바
    TimeBar *timeBar;
    
    // 레벨 표시
    cocos2d::Label *leveLabel;
    cocos2d::Label *hitCountLabel;
    int hitCount;
};

#endif /* GameView_hpp */
