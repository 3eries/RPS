//
//  PausePopup.hpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#ifndef PausePopup_hpp
#define PausePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class PausePopup : public SBBasePopup {
public:
    enum class MenuType {
        NONE,
        RESUME,
        HOME,
    };
    
public:
    CREATE_FUNC(PausePopup);
    ~PausePopup();
    
private:
    PausePopup();
    
    bool init() override;
    void onExit() override;
    
    void initBg();
    void initTopMenu();
    void initCenterMenu();
    
private:
    cocos2d::Node *contentsLayer;
    
    CC_SYNTHESIZE(std::function<void(MenuType)>, onClickMenuListener,
                  OnClickMenuListener);
};

#endif /* PausePopup_hpp */
