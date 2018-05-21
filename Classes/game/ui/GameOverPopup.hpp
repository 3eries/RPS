//
//  GameOverPopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameOverPopup_hpp
#define GameOverPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class GameOverPopup : public SBBasePopup {
public:
    enum class MenuType {
        NONE,
        RESTART,
        HOME,
    };
    
public:
    CREATE_FUNC(GameOverPopup);
    ~GameOverPopup();
    
private:
    GameOverPopup();
    
    bool init() override;
    void onExit() override;
    
    void initBg();
    void initMenu();
    
private:
    cocos2d::Node *contentLayer;
    
    CC_SYNTHESIZE(std::function<void(MenuType)>, onClickMenuListener,
                  OnClickMenuListener);
};

#endif /* GameOverPopup_hpp */
