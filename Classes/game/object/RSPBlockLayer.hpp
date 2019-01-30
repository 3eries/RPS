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

#include "Define.h"

#include "../GameManager.hpp"
#include "Man.hpp"

class RSPBlock;

class RSPBlockLayer : public cocos2d::Node, public GameListener {
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
    
    RSPType getBlockType(int i);
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGameOver() override;
    void onGameModeChanged(GameMode mode) override;
    
public:
    void resetBlocks();
    
    RSPBlock*              getFirstBlock();
    std::vector<RSPBlock*> getSortedBlocks();
    
    void hitBlock(RSPBlock *block, RSPType btnType, bool isManOnLeft);
    void misBlock(RSPBlock *block);
    void drawBlock(RSPBlock *block, bool isManOnLeft,
                   DrawAnimEventListener eventListener);
    
    std::string toString();
    
private:
    GameManager *gameMgr;
    
    std::vector<RSPBlock*> blocks;
    int blockIndex;
    
    CC_SYNTHESIZE(std::function<void(RSPBlock*)>, onFirstBlockChangedListener,
                  OnFirstBlockChangedListener);
};

#endif /* RSPBlockLayer_hpp */
