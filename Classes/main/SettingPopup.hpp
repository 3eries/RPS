//
//  SettingPopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#ifndef SettingPopup_hpp
#define SettingPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class SettingPopup : public BasePopup {
public:
    enum Tag {
        CLOSE = 100,
        WORLD_RANKING,
        RESTORE_PURCHASE,
        REMOVE_ADS,
    };
    
public:
    CREATE_FUNC(SettingPopup);
    ~SettingPopup();
    
private:
    SettingPopup();
    
    bool init() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void performListener(Tag tag);
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(std::function<void(Tag)>, onClickMenuListener,
                  OnClickMenuListener);
    cocos2d::Node *stoneBg;
};

#endif /* SettingPopup_hpp */
