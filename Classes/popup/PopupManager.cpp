//
//  PopupManager.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#include "PopupManager.hpp"

#include "SceneManager.h"

USING_NS_CC;
using namespace std;

static PopupManager *instance = nullptr;
PopupManager* PopupManager::getInstance() {
    
    if( !instance ) {
        instance = new PopupManager();
    }
    
    return instance;
}

void PopupManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

PopupManager::PopupManager() {
}

PopupManager::~PopupManager() {
    
    popups.clear();
    listeners.clear();
}

/**
 * 팝업 추가
 */
void PopupManager::addPopup(BasePopup *popup) {
    
    CCASSERT(popup != nullptr, "PopupManager::addPopup error: popup must be not null");
    CCASSERT(popup->getType() != BasePopup::Type::NONE, "PopupManager::addPopup error: type must be not none");
    CCASSERT(!popups.contains(popup), "PopupManager::addPopup error: popup already added");
    
    popups.pushBack(popup);
    
    CCLOG("PopupManager::addPopup type: %d count: %d", (int)popup->getType(), (int)getPopupCount());
}

/**
 * 팝업 제거
 */
void PopupManager::removePopup(BasePopup *popup) {
    
    CCASSERT(popup != nullptr, "PopupManager::removePopup error: popup must be not null");
    
    CCLOG("PopupManager::removePopup type: %d count: %d", (int)popup->getType(), (int)getPopupCount()-1);
    
    if( popups.contains(popup) ) {
        popups.eraseObject(popup);
    }
}

/**
 * 해당 타입의 팝업 반환
 */
BasePopup* PopupManager::getPopup(BasePopup::Type type) {
    
    for( auto popup : popups ) {
        if( popup->getType() == type ) {
            return popup;
        }
    }
    
    return nullptr;
}

/**
 * 팝업 갯수 반환
 */
size_t PopupManager::getPopupCount() {
    return popups.size();
}

/**
 * 가장 앞에 나와 있는 팝업 반환
 */
BasePopup* PopupManager::getFrontPopup() {
    
    return popups.at(popups.size()-1);
}

/**
 * 해당 타입의 팝업 존재 여부 반환
 */
bool PopupManager::exists(BasePopup::Type type) {
    
    return getPopup(type) != nullptr;
}

/**
 * 리스너 등록
 */
void PopupManager::addListener(PopupListener *listener) {
    
    CCASSERT(listener != nullptr, "PopupManager::addListener error: listener must be not null");
    
    if( !listeners.contains(listener) ) {
        listeners.pushBack(listener);
    }
    
    CCLOG("PopupManager::addListener count: %d", (int)listeners.size());
}

void PopupManager::addListener(Node *target, PopupListener *listener) {
    
    CCASSERT(target != nullptr, "PopupManager::addListener error: target must be not null");
    CCASSERT(listener != nullptr, "PopupManager::addListener error: listener must be not null");
    
    listener->setTarget(target);
    addListener(listener);
}

/**
 * 리스너 제거
 */
void PopupManager::removeListener(PopupListener *listener) {
    
    CCASSERT(listener != nullptr, "PopupManager::removeListener error: listener must be not null");
    
    if( listeners.contains(listener) ) {
        listeners.eraseObject(listener);
    }
    
    CCLOG("PopupManager::removeListener count: %d", (int)listeners.size());
}

void PopupManager::removeListener(Node *target) {
    
    CCASSERT(target != nullptr, "PopupManager::removeListener error: target must be not null");
    
    auto removeListeners = SBCollection::find(listeners, [=](PopupListener *listener) -> bool {
        return listener->getTarget() == target;
    });
    
    for( auto listener : removeListeners ) {
        listeners.eraseObject(listener);
    }
    
    CCLOG("PopupManager::removeListener count: %d", (int)listeners.size());
}

/**
 * 팝업 이벤트 전달
 */
void PopupManager::dispatchEvent(BasePopup *popup, PopupEventType eventType) {
    
    for( auto listener : listeners ) {
        if( listener->onEvent ) {
            listener->onEvent(popup, eventType);
        }
    }
}


