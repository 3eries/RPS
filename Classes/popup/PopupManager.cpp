//
//  PopupManager.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#include "PopupManager.hpp"

#include "RSP.h"
#include "SceneManager.h"

#include "SettingPopup.hpp"
#include "RankingPopup.hpp"
#include "ShopPopup.hpp"
#include "GetCharacterPopup.hpp"

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

string PopupManager::getPopupInfo() {
    
    auto getPopupName = [](BasePopup::Type type) -> string {
        switch( type ) {
            case BasePopup::Type::NONE:              return "NONE";
            case BasePopup::Type::LOADING_BAR:       return "LOADING_BAR";
            case BasePopup::Type::EXIT_APP:          return "EXIT_APP";
            case BasePopup::Type::REVIEW:            return "REVIEW";
            case BasePopup::Type::CREDIT:            return "CREDIT";
            case BasePopup::Type::SHOP:              return "SHOP";
            case BasePopup::Type::GET_CHARACTER:     return "GET_CHARACTER";
            case BasePopup::Type::SETTING:           return "SETTING";
            case BasePopup::Type::RANKING:           return "RANKING";
            case BasePopup::Type::NEW_RECORD:        return "NEW_RECORD";
            case BasePopup::Type::PAUSE:             return "PAUSE";
            case BasePopup::Type::GAME_OVER:         return "GAME_OVER";
            case BasePopup::Type::CONTINUE:          return "CONTINUE";
            default: break;
        };
        
        return "INVALID";
    };
    
    auto popups = getInstance()->popups;
    string str = "PopupInfo\n[\n";
    
    for( int i = popups.size()-1; i >= 0; --i ) {
        auto popup = popups.at(i);
        str += STR_FORMAT("\t%s isVisible: %d", getPopupName(popup->getType()).c_str(), popup->isVisible()) + "\n";
    }
    
    str += "]";
    
    return str;
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
    
    auto popups = getInstance()->popups;
    
    for( auto popup : popups ) {
        if( popup->getType() == type ) {
            return popup;
        }
    }
    
    return nullptr;
}

/**
 * 가장 앞에 나와 있는 팝업 반환
 */
BasePopup* PopupManager::getFrontPopup() {
    
    auto popups = getInstance()->popups;
    
    for( int i = popups.size()-1; i >= 0; --i ) {
        auto popup = popups.at(i);
        
        if( popup->isVisible() ) {
            return popup;
        }
    }
    
    return nullptr;
}

/**
 * 모든 팝업 갯수 반환
 */
size_t PopupManager::getPopupCount() {
    return getInstance()->popups.size();
}

/**
 * 해당 타입의 팝업 갯수를 반환합니다
 */
size_t PopupManager::getPopupCount(BasePopup::Type type) {
    
    auto popups = getInstance()->popups;
    size_t cnt = 0;
    
    for( auto popup : popups ) {
        if( popup->getType() == type ) {
            cnt++;
        }
    }
    
    return cnt;
}

/**
 * 큰 팝업 갯수 반환
 * ex) 랭킹, 게임 오버
 */
size_t PopupManager::getLargePopupCount() {
    
    auto popups = getInstance()->popups;
    size_t cnt = 0;
    
    for( auto popup : popups ) {
        if( popup->getType() == BasePopup::Type::RANKING ||
            popup->getType() == BasePopup::Type::NEW_RECORD ||
            popup->getType() == BasePopup::Type::GAME_OVER ) {
            cnt++;
        }
    }
    
    return cnt;
}

/**
 * 해당 타입의 팝업 존재 여부 반환
 */
bool PopupManager::exists(BasePopup::Type type) {
    
    return getPopup(type) != nullptr;
}

/**
 * 팝업 노출
 */
void PopupManager::show(OnPopupEvent onEventListener, BasePopup::Type type) {
    
    auto popup = createPopup(type);
    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
        
        if( onEventListener ) {
            onEventListener(sender, eventType);
        }
    });
    popup->setOnDismissListener([=](Node*) {
    });
    
    SceneManager::getInstance()->getScene()->addChild(popup, PopupZOrder::BOTTOM);
    
    popup->runEnterAction();
}

void PopupManager::show(BasePopup::Type type) {

    show(nullptr, type);
}

/**
 * 캐릭터 획득 팝업 노출
 */
void PopupManager::showGetCharacterPopup(const Characters &characters) {
   
    auto &getCharacterPopups = getInstance()->getCharacterPopups;
    
    // 이전 리스트 클리어
    getCharacterPopups.clear();
    
    for( auto character : characters ) {
        auto popup = GetCharacterPopup::create(character);
        popup->setVisible(false);
        popup->setIgnoreDismissAction(true); // 퇴장 연출 off
        popup->setOnDismissListener([=](Node*) {
    
            // 노출된 팝업 제거
            auto &getCharacterPopups = getInstance()->getCharacterPopups;
            getCharacterPopups.erase(getCharacterPopups.begin());
            
            if( getCharacterPopups.size() == 0 ) {
                return;
            }
            
            // 다음 팝업 노출
            auto nextPopup = getCharacterPopups[0];
            nextPopup->setVisible(true);
            nextPopup->onEnterActionFinished();
        });
        SceneManager::getInstance()->getScene()->addChild(popup, PopupZOrder::TOP);
        
        getCharacterPopups.push_back(popup);
    }

    // 첫번째 팝업 노출
    auto firstPopup = getCharacterPopups[0];
    firstPopup->setVisible(true);
    firstPopup->runEnterAction();
    
    // 마지막 팝업 퇴장 연출 나오도록
    auto lastPopup = getCharacterPopups[getCharacterPopups.size()-1];
    lastPopup->setIgnoreDismissAction(false);
}

/**
 * 팝업 크로스
 * 1번 퇴장 -> 2번 등장 -> 2번 퇴장 -> 3번 등장
 */
void PopupManager::cross(OnPopupEvent onEventListener,
                         BasePopup *popup1, BasePopup *popup2, BasePopup *popup3) {
    
    CCASSERT(popup1 != nullptr, "PopupManager::cross error: invalid popup1.");
    CCASSERT(popup2 != nullptr, "PopupManager::cross error: invalid popup2.");
    
    vector<BasePopup*> crossPopups({ popup1, popup2, });
    
    if( popup3 ) {
        crossPopups.push_back(popup3);
    }
    
    auto commonMenu = SceneManager::getCommonMenu();
    const float POPUP_EFFECT_TIME_SCALE = 0.7f;
    
    // 팝업 등/퇴장 시간 스케일
    for( auto popup : crossPopups ) {
        popup->setEnterTimeScale(POPUP_EFFECT_TIME_SCALE);
        popup->setExitTimeScale(POPUP_EFFECT_TIME_SCALE);
    }
    
    // 팝업 리스너 등록
    auto listener = PopupListener::create();
    listener->retain();
    listener->onEvent = [=](Node *sender, PopupEventType eventType) {
        
        bool found = false;
        
        for( auto popup : crossPopups ) {
            if( popup == sender ) {
                found = true;
                break;
            }
        }
        
        if( !found ) {
            return;
        }
        
        if( onEventListener ) {
            onEventListener(sender, eventType);
        }
        
        auto popup = dynamic_cast<BasePopup*>(sender);
        
        switch( eventType ) {
            case PopupEventType::ENTER: {
            } break;
                
            case PopupEventType::ENTER_ACTION_FINISHED: {
                popup->setEnterTimeScale(1);
                
                // 3번 등장 후 리스너 제거
                if( popup3 && popup == popup3 ) {
                    getInstance()->removeListener(listener);
                }
                
            } break;
                
            case PopupEventType::EXIT_ACTION: {
            } break;
                
            case PopupEventType::EXIT_ACTION_FINISHED: {
                popup->setExitTimeScale(1);
                
                // Step 3. 2번 퇴장 -> 3번 등장
                if( popup == popup2 ) {
                    if( popup3 ) {
                        popup3->runEnterAction();
                    }
                    // 3번 없음, 리스너 제거
                    else {
                        getInstance()->removeListener(listener);
                    }
                }
                
            } break;
                
            default: break;
        }
    };
    
    getInstance()->addListener(listener);
    
    // Step 1. 1번 팝업 퇴장
    popup1->runExitAction([=]() {
        
        // 3번 팝업이 있는 경우 1번 팝업 제거
//        if( popup1 != popup3 ) {
//            popup1->dismiss();
//        }
        
        // Step 2. 2번 팝업 등장
        SceneManager::getScene()->addChild(popup2, PopupZOrder::BOTTOM);
        popup2->runEnterAction();
    });
    
    commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::BACK);
}

void PopupManager::cross(BasePopup *popup1, BasePopup *popup2, BasePopup *popup3) {
    
    cross(nullptr, popup1, popup2, popup3);
}

BasePopup* PopupManager::createPopup(BasePopup::Type type) {
    
    switch( type ) {
        case BasePopup::Type::SHOP:                  return ShopPopup::create();
        case BasePopup::Type::RANKING:               return RankingPopup::create();
        default:
            return nullptr;
    }
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


