//
//  GameLayer.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameLayer_hpp
#define GameLayer_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RSP.h"

class RSPBlock;

class GameLayer : public cocos2d::Node {
public:
    CREATE_FUNC(GameLayer);
    ~GameLayer();
    
private:
    GameLayer();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBlocks();
    void initButtons();
    void initLabels();
    
private:
    void onClickButton(RSPType type);
    
    void hitBlock(RSPBlock *block);
    void misBlock(RSPBlock *block);
    
    void runHitBlockEffect(RSPBlock *hitBlock);
    
    void alignBlock(int i, RSPBlock *block);
    cocos2d::Vec2 getBlockPosition(int i);
    
private:
    CC_SYNTHESIZE(SBCallback, onGameOverListener, OnGameOverListener);
    
    cocos2d::Node *blockLayer;
    std::vector<RSPBlock*> blocks;
    int blockIndex;
    
    cocos2d::Label *hitCountLabel;
    int hitCount;
};

#endif /* GameLayer_hpp */
