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

class ContinuePopup : public SBBasePopup {
public:
    CREATE_FUNC(ContinuePopup);
    ~ContinuePopup();
    
private:
    ContinuePopup();
    
    bool init() override;
    void onExit() override;
    
    void initBg();
    void initMenu();
    
    void close();
    
    void countdown();
    void timeOut();
    
private:
    CC_SYNTHESIZE(SBCallback, onClosedListener, OnClosedListener);
    CC_SYNTHESIZE(SBCallback, onVideoListener, OnVideoListener);
    CC_SYNTHESIZE(SBCallback, onTimeOutListener, OnTimeOutListener);
    
    cocos2d::Node *contentLayer;
    
    float elapsed;
    cocos2d::Label *countdownLabel;
};

#endif /* ContinuePopup_hpp */
