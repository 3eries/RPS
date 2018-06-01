//
//  BaseTestScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef BaseTestScene_hpp
#define BaseTestScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class BaseTestScene : public cocos2d::Scene {
public:
    CREATE_FUNC(BaseTestScene);
    virtual ~BaseTestScene();
    
protected:
    BaseTestScene();
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExit() override;
    
    virtual void onBack();
};

#endif /* BaseTestScene_hpp */
