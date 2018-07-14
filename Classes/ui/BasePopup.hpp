//
//  BasePopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 5..
//

#ifndef BasePopup_hpp
#define BasePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class PopupManager;

class BasePopup : public SBBasePopup {
public:
    enum class Type {
        NONE,
        EXIT_APP,
        CREDIT,
        SETTING,
        RANKING,
        NEW_RECORD,
        PAUSE,
        GAME_OVER,
        CONTINUE,
    };
    
public:
    virtual ~BasePopup();
    
protected:
    BasePopup(Type type);
    
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;
    
    virtual void setBackgroundColor(const cocos2d::Color4B &color);
    
public:
    virtual void dismissWithAction(SBCallback onFinished = nullptr);
    
    virtual void runEnterAction(SBCallback onFinished);
    virtual void runExitAction(SBCallback onFinished);

    virtual void onEnterActionFinished();
    virtual void onExitActionFinished();
    
    virtual void runSlideAction(SBCallback onFinished,
                                float duratoin, cocos2d::Vec2 from, cocos2d::Vec2 to);
    virtual void runSlideInAction(SBCallback onFinished, float duratoin);
    virtual void runSlideOutAction(SBCallback onFinished, float duratoin);
    
protected:
    PopupManager *popupMgr;
    CC_SYNTHESIZE_READONLY(Type, type, Type);
};

#endif /* BasePopup_hpp */
