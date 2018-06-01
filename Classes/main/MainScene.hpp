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

class MainScene : public cocos2d::Scene, public SBNodeListener {
private:
    typedef enum {
        BTN_START = 100,
        BTN_REMOVE_ADS,
    } Tag;
    
public:
    CREATE_FUNC(MainScene);
    ~MainScene();
    
private:
    MainScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initMenu();
    
    void onClick(cocos2d::Node *sender) override;
    
private:
    void replaceGameScene();
};

#endif /* MainScene_hpp */
