//
//  ShopPopup.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 22..
//

#ifndef ShopPopup_hpp
#define ShopPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "CharacterManager.hpp"

class PackageNavigator;
class CharacterView;

class ShopPopup : public BasePopup {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    
private:
    enum Tag {
        BTN_LEFT,
        BTN_RIGHT,
        BTN_SELECT,
        BTN_BUY,
        BTN_VIEW_ADS,
    };
    
public:
    static ShopPopup* create();
    ~ShopPopup();
    
private:
    ShopPopup();
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void initNavigator();
    void initCharacter();
    void initBottomMenu();
    
    void onClick(cocos2d::Node *sender) override;
    void onPackageChanged(Package pack);
    
    void setCharacter(int i);
    
private:
    Character getCharacter();
    
public:
    void dismiss() override;
    
    void runEnterAction(float duration, SBCallback onFinished) override;
    void runEnterAction(SBCallback onFinished = nullptr) override;
    
    void runExitAction(float duration, SBCallback onFinished) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CharacterManager *charMgr;
    
    Package pack;
    int characterIndex;
    
    PackageNavigator *navigator;
    CharacterView *characterView;
    cocos2d::Node *bottomMenu;
};

#endif /* ShopPopup_hpp */
