//
//  RSPButton.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 15..
//

#ifndef RSPButton_hpp
#define RSPButton_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "Define.h"

class RSPButton : public cocos2d::Node {
public:
    static std::string getButtonImageFile(RSPType type);
    
public:
    static RSPButton* create(RSPType type);
    ~RSPButton();
    
private:
    RSPButton(RSPType type);
    
    bool init() override;
    
    void initImage();
    void initTouch();
    
public:
    void setTouchEnabled(bool isTouchEnabled);
    
private:
    CC_SYNTHESIZE_READONLY(RSPType, type, Type);
    bool isTouchEnabled;
    CC_SYNTHESIZE(SBCallbackNode, onClickListener, OnClickListener);
    
    CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, img, Image);
};

#endif /* RSPButton_hpp */
