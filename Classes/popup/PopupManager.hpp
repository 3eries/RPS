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

#include "PopupListener.hpp"

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
    
    static BasePopup* getPopup(BasePopup::Type type);
    static BasePopup* getFrontPopup();
    static size_t     getPopupCount();
    static size_t     getPopupCount(BasePopup::Type type);
    static size_t     getLargePopupCount();
    static bool       exists(BasePopup::Type type);
    
    bool       exists(BasePopup::Type type);
    
    void addListener(PopupListener *listener);
    void addListener(cocos2d::Node *target, PopupListener *listener);
    
    void removeListener(PopupListener *listener);
    void removeListener(cocos2d::Node *target);
    
    void dispatchEvent(BasePopup *popup, PopupEventType eventType);
    
private:
    CC_SYNTHESIZE_READONLY(cocos2d::Vector<BasePopup*>, popups, Popups);
    cocos2d::Vector<PopupListener*> listeners;
};

#endif /* PopupManager_hpp */
