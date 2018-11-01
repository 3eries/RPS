//
//  BaseScene.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 19..
//

#ifndef BaseScene_hpp
#define BaseScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "CommonMenu.hpp"

class BaseScene : public cocos2d::Scene, public SBNodeListener {
protected:
    BaseScene();
    virtual ~BaseScene();
    
protected:
    virtual bool init() override;
    virtual void initCommonMenu();
    
    virtual bool onBackKeyReleased() override { return true; }
    virtual void onClick(cocos2d::Node *sender) override {}
    
protected:
    CC_SYNTHESIZE_READONLY(CommonMenu*, commonMenu, CommonMenu);
};

#endif /* BaseScene_hpp */
