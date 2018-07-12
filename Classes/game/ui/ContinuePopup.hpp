//
//  ContinuePopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef ContinuePopup_hpp
#define ContinuePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ContinuePopup : public BasePopup {
private:
    enum Tag {
        // IMG_TITLE = 100,
        // BTN_VIDEO,
    };
    
public:
    CREATE_FUNC(ContinuePopup);
    ~ContinuePopup();
    
private:
    ContinuePopup();
    
    bool init() override;
    void onExit() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void countdown();
    void timeOut();
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    CC_SYNTHESIZE(SBCallback, onVideoListener, OnVideoListener);
    CC_SYNTHESIZE(SBCallback, onTimeOutListener, OnTimeOutListener);
    
    int count;
    spine::SkeletonAnimation *countdownAnim;
};

#endif /* ContinuePopup_hpp */
