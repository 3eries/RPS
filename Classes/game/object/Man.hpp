//
//  Man.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#ifndef Man_hpp
#define Man_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "RSP.h"

class Man : public cocos2d::Node {
public:
    enum class Position {
        LEFT,
        RIGHT,
    };
    
public:
    CREATE_FUNC(Man);
    ~Man();
    
private:
    Man();
    
    bool init() override;
    void initImage();
    void initFeverGage();
    
    void setManPosition(Position pos);
    
public:
    void showdown(/*RSPType type*/);
    void hit(RSPType type);
    
private:
    CC_SYNTHESIZE(RSPType, lastHitType, LastHitType);
    CC_SYNTHESIZE_READONLY(Position, manPosition, ManPosition);
    
private:
    SBAnimationSprite *img;
//    cocos2d::Sprite *gage;
    
    struct FeverGage {
        cocos2d::Node *bg;
        cocos2d::ProgressTimer *gage;
    };
    
    FeverGage feverGage;
};

#endif /* Man_hpp */
