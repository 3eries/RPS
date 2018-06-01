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

class RSPBlock;
class RSPButton;
class Man;
class TimeBar;

class GameView : public cocos2d::Node, public GameListener {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
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
    
// GameListener
private:
    
private:
    void onClickButton(RSPType type);
    
    void hitBlock(RSPBlock *block, RSPType btnType);
    void misBlock(RSPBlock *block);
    void drawBlock(RSPBlock *block);
    
    void runHitBlockEffect(RSPBlock *hitBlock);
    
    void alignBlock(int i, RSPBlock *block);
    cocos2d::Vec2 getBlockPosition(int i);
    
private:
    GameManager *gameMgr;
    
    cocos2d::Node *blockLayer;
    std::vector<RSPBlock*> blocks;
    int blockIndex;
    
    std::vector<RSPButton*> rspButtons;
    
    Man *man;
    TimeBar *timeBar;
    
    cocos2d::Label *hitCountLabel;
    int hitCount;
};

#endif /* GameView_hpp */
