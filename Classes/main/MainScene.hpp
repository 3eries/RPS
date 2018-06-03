//
//  MainScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#ifndef MainScene_hpp
#define MainScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../base/ViewManager.hpp"

class MainScene : public cocos2d::Scene, public ViewListener, public SBNodeListener {
public:
    CREATE_FUNC(MainScene);
    ~MainScene();
    
private:
    MainScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void onViewChanged(ViewType viewType) override;
    
    void initBg();
    
private:
    cocos2d::Sprite *banner;
};

#endif /* MainScene_hpp */
