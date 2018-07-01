//
//  CreditPopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#ifndef CreditPopup_hpp
#define CreditPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class CreditPopup : public SBBasePopup {
public:
    static void show(cocos2d::Node *parent, int zOrder);
    
public:
    CREATE_FUNC(CreditPopup);
    ~CreditPopup();
    
private:
    CreditPopup();
    
    bool init() override;
    void onExit() override;
    
    void initBg();
    void initCredit();
    
    void onActionCompleted();
    
    void close();
    
private:
    CC_SYNTHESIZE(SBCallback, onClosedListener, OnClosedListener);
    cocos2d::Node *contentView;
};

#endif /* CreditPopup_hpp */
