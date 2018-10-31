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
#include "PopupDefine.h"
#include "PopupListener.hpp"

#include "CharacterManager.hpp"

class PopupManager {
public:
    static PopupManager* getInstance();
    static void destroyInstance();
    ~PopupManager();
    
    static std::string getPopupInfo();
    
private:
    PopupManager();
    
public:
    void addPopup(BasePopup *popup);
    void removePopup(BasePopup *popup);
    
    static BasePopup* getPopup(PopupType type);
    static BasePopup* getFrontPopup();
    static size_t     getPopupCount();
    static size_t     getPopupCount(PopupType type);
    static size_t     getLargePopupCount();
    static bool       exists(PopupType type);
    
public:
    static void show(OnPopupEvent onEventListener, PopupType type);
    static void show(PopupType type);
    static void showGetCharacterPopup(const Characters &characters);
    
    static void cross(OnPopupEvent onEventListener,
                      BasePopup *popup1, BasePopup *popup2,
                      BasePopup *popup3 = nullptr);
    static void cross(BasePopup *popup1, BasePopup *popup2,
                      BasePopup *popup3 = nullptr);
    
    static BasePopup* createPopup(PopupType type);
    
public:
    void addListener(PopupListener *listener);
    void addListener(cocos2d::Node *target, PopupListener *listener);
    
    void removeListener(PopupListener *listener);
    void removeListener(cocos2d::Node *target);
    
    void dispatchEvent(BasePopup *popup, PopupEventType eventType);
    
private:
    CC_SYNTHESIZE_READONLY(cocos2d::Vector<BasePopup*>, popups, Popups);
    cocos2d::Vector<PopupListener*> listeners;
    
    // 캐릭터 획득 팝업 리스트
    // 팝업 소멸 시 리스트에서도 제거된다
    std::vector<BasePopup*> getCharacterPopups;
};

#endif /* PopupManager_hpp */
