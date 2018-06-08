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

#include "RSP.h"

class RSPBlock : public cocos2d::Sprite {
public:
    static RSPType getRandomType();
    
public:
    static RSPBlock* create(RSPType type);
    static RSPBlock* createRandomBlock();
    ~RSPBlock();
    
private:
    RSPBlock();
  
    bool init(RSPType type);
    
public:
    void setBlock(RSPType type);
    void changeRandomBlock();
    
    bool isEqualBlock(RSPType type);
    
    RSPBlock* clone();
    
    std::string toString();
    
private:
    CC_SYNTHESIZE(RSPType, type, Type);
};

#endif /* RSPBlock_hpp */
