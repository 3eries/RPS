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

#include "BaseScene.hpp"

class MainScene : public BaseScene {
private:
    enum Tag {
        VIP_MARK,
        
        BTN_TITLE,
        BTN_REMOVE_ADS,
        BTN_TEST,
    };
    
public:
    CREATE_FUNC(MainScene);
    ~MainScene();
    
private:
    MainScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    bool onBackKeyReleased() override;
    
    void initBg();
    void initMenu();
    
    void initIAPListener();
    void initPopupListener();
    
private:
    void replaceGame();
    
    void onClick(cocos2d::Node *sender) override;
    
private:
    cocos2d::Node *contentView; // 메인 Scene 컨텐트 뷰
};

#endif /* MainScene_hpp */
