//
//  MainMenu.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 3..
//

#ifndef MainMenu_hpp
#define MainMenu_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class MainMenu : public cocos2d::Node, public SBNodeListener {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    
    enum Tag {
        START               = 100,
        RANKING_LOCAL,
        RANKING_WORLD,
        SHOP,
        SETTING,
    };
    
public:
    CREATE_FUNC(MainMenu);
    ~MainMenu();
    
private:
    MainMenu();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initTopMenu();
    void initBottomMenu();
    
    void onClick(cocos2d::Node *sender) override;
    
public:
//    void openMenu();
//    void closeMenu();
    
    void setTouchEnabled(bool enabled);
    void setRankingButton(Tag tag);
    
    void showRankingPopup();
    void showSettingPopup();
    
    void runEnterAction();
    void runExitAction();
    
    void runTopMenuEnterAction(float duration = SLIDE_IN_DURATION);
    void runTopMenuExitAction(float duration = SLIDE_OUT_DURATION);
    
    void runBottomMenuEnterAction();
    void runBottomMenuExitAction();
    
    void runMenuMoveAction(cocos2d::Node *menu,
                           cocos2d::Vec2 from, cocos2d::Vec2 to, float duration);
    
private:
    CC_SYNTHESIZE(std::function<bool(Tag)>, onClickListener, OnClickListener);
    
    cocos2d::Node *touchLockNode;
    
    cocos2d::Node *topMenu;
    cocos2d::Node *bottomMenu;
};

#endif /* MainMenu_hpp */
