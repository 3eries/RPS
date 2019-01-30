//
//  RSPBlock.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 15..
//

#ifndef RSPBlock_hpp
#define RSPBlock_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "Define.h"

class RSPBlock : public cocos2d::Sprite {
public:
    static const int INVALID_INDEX = -1;
    
public:
    static RSPBlock* create(RSPType type);
    static RSPBlock* createRandomBlock();
    ~RSPBlock();
    
private:
    RSPBlock();
  
    bool init(RSPType type);
    
public:
    void setBlock(RSPType type);
    void setIndex(int i);
    
    void changeRandomBlock();
    void refreshPosition();
    
    void downWithAction();
    
    void runHitAction(bool isManOnLeft);
    void runDrawAction(bool isManOnLeft, DrawAnimEventListener eventListener);
    
    static cocos2d::Vec2 getBlockPosition(RSPType type, int i);
    static float         getBlockPositionX(RSPType type);
    static float         getBlockPositionY(int i);
    
    static std::string   getBlockImageFile(RSPType type, bool flippedX = false);
    static RSPType       getRandomType();

    RSPBlock* clone();
    std::string toString();
    
private:
    CC_SYNTHESIZE(RSPType, type, Type);
    CC_SYNTHESIZE_READONLY(int, idx, Index);
};

#endif /* RSPBlock_hpp */
