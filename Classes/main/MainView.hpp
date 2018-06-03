//
//  MainView.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#ifndef MainView_hpp
#define MainView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../base/ViewManager.hpp"

class MainView : public cocos2d::Node, public ViewListener, public SBNodeListener {
private:
    typedef enum {
        BTN_START = 100,
        BTN_REMOVE_ADS,
    } Tag;
    
public:
    CREATE_FUNC(MainView);
    ~MainView();
    
private:
    MainView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void onViewChanged(ViewType viewType) override;
    
    void initBg();
    void initMenu();
    
    void onClick(cocos2d::Node *sender) override;
    
private:
    void replaceGame();
};

#endif /* MainView_hpp */
