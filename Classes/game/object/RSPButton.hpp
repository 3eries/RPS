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

#include "RSP.h"

class RSPButton : public cocos2d::ui::Widget {
public:
    static RSPButton* create(RSPType type);
    ~RSPButton();
    
private:
    RSPButton(RSPType type);
    
    bool init() override;
    
    void initImage();
    void initTouch();
    
private:
    CC_SYNTHESIZE_READONLY(RSPType, type, Type);
    CC_SYNTHESIZE(SBCallbackNode, onClickListener, OnClickListener);
    
    cocos2d::Sprite *img;
};

#endif /* RSPButton_hpp */
