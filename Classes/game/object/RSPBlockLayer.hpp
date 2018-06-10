//
//  RSPBlockLayer.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 7..
//

#ifndef RSPBlockLayer_hpp
#define RSPBlockLayer_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RSP.h"
#include "ViewManager.hpp"

#include "../GameManager.hpp"
#include "Man.hpp"

class RSPBlock;

class RSPBlockLayer : public cocos2d::Node, public ViewListener, public GameListener {
public:
    CREATE_FUNC(RSPBlockLayer);
    ~RSPBlockLayer();
    
private:
    RSPBlockLayer();
    
private:
    bool init() override;
    void onEnter() override;
    void onExit() override;
    
    void initBlocks();
    void resetBlocks();
    
    void onViewChanged(ViewType viewType) override;
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGameOver() override;
    void onGameModeChanged(GameMode mode) override;
    
public:
    RSPBlock* getFirstBlock();
    
    void hitBlock(RSPBlock *block, RSPType btnType,
                  Man::Position manPosition);
    void misBlock(RSPBlock *block);
    void drawBlock(RSPBlock *block);
    
    void runHitBlockEffect(RSPBlock *hitBlock, Man::Position manPosition);
    
    std::string toString();
    
private:
    RSPType getBlockType(int i);
    
    void alignBlock(int i, RSPBlock *block);
    cocos2d::Vec2 getBlockPosition(int i);
    
private:
    GameManager *gameMgr;
    
    std::vector<RSPBlock*> blocks;
    int blockIndex;
};

#endif /* RSPBlockLayer_hpp */
