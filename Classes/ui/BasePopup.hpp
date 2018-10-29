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

#include "PopupListener.hpp"

class PopupManager;

class BasePopup : public SBBasePopup {
public:
    enum class Type {
        NONE,
        LOADING_BAR,
        EXIT_APP,
        REVIEW,
        CREDIT,
        SHOP,
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
    
    virtual void onPopupEvent(PopupEventType eventType);
    
    virtual void setBackgroundColor(const cocos2d::Color4B &color);
    
public:
    virtual void dismissWithAction(SBCallback onFinished = nullptr);

    virtual void runEnterAction(float duration, SBCallback onFinished = nullptr);
    virtual void runEnterAction(SBCallback onFinished = nullptr);
    
    virtual void runExitAction(float duration, SBCallback onFinished = nullptr);
    virtual void runExitAction(SBCallback onFinished = nullptr);

    virtual void onEnterActionFinished();
    virtual void onExitActionFinished();
    
    virtual void runSlideAction(SBCallback onFinished,
                                float duratoin, cocos2d::Vec2 from, cocos2d::Vec2 to);
    virtual void runSlideInAction(SBCallback onFinished, float duratoin);
    virtual void runSlideOutAction(SBCallback onFinished, float duratoin);
    
protected:
    PopupManager *popupMgr;
    CC_SYNTHESIZE_READONLY(Type, type, Type);
    CC_SYNTHESIZE(OnPopupEvent, onPopupEventListener, OnPopupEventListener);
    
    CC_SYNTHESIZE(float, enterTimeScale, EnterTimeScale);       // 등장 연출 타임 스케일
    CC_SYNTHESIZE(float, exitTimeScale,  ExitTimeScale);        // 퇴장 연출 타임 스케일
    
    SB_SYNTHESIZE_BOOL(runningEnterAction, RunningEnterAction); // 등장 연출 진행 여부
    SB_SYNTHESIZE_BOOL(runningExitAction, RunningExitAction);   // 퇴장 연출 진행 여부
};

#endif /* BasePopup_hpp */
