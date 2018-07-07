//
//  PopupManager.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#ifndef PopupManager_hpp
#define PopupManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "BasePopup.hpp"

#pragma mark- PopupListener

enum class PopupEventType {
    ENTER = 0,                  // 팝업 등장
    EXIT,                       // 팝업 퇴장
    ENTER_ACTION,               // 팝업 등장 연출 시작
    EXIT_ACTION,                // 팝업 퇴장 연출 시작
    ENTER_ACTION_FINISHED,      // 팝업 등장 연출 완료
    EXIT_ACTION_FINISHED,       // 팝업 퇴장 연출 완료
};

class PopupListener : public cocos2d::Ref {
public:
    SB_REF_CREATE_FUNC(PopupListener);
    
    PopupListener() : target(nullptr), onEvent(nullptr) {}
    
    CC_SYNTHESIZE(cocos2d::Node*, target, Target);
    std::function<void(BasePopup*, PopupEventType)> onEvent;
};

#pragma mark- PopupManager

static const int POPUP_ZORDER = SBZOrder::MIDDLE;

class PopupManager {
public:
    static PopupManager* getInstance();
    static void destroyInstance();
    ~PopupManager();
    
private:
    PopupManager();
    
public:
    void addPopup(BasePopup *popup);
    void removePopup(BasePopup *popup);
    
    BasePopup* getPopup(BasePopup::Type type);
    size_t     getPopupCount();
    bool       exists(BasePopup::Type type);
    
    void addListener(PopupListener *listener);
    void addListener(cocos2d::Node *target, PopupListener *listener);
    
    void removeListener(PopupListener *listener);
    void removeListener(cocos2d::Node *target);
    
    void dispatchEvent(BasePopup *popup, PopupEventType eventType);
    
private:
    cocos2d::Vector<BasePopup*> popups;
    cocos2d::Vector<PopupListener*> listeners;
};

#endif /* PopupManager_hpp */
