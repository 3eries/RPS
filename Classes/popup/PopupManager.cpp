//
//  PopupManager.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#include "PopupManager.hpp"

#include <spine/spine-cocos2dx.h>

#include "Define.h"
#include "SceneManager.h"

#include "SettingPopup.hpp"
#include "RankingPopup.hpp"
#include "ShopPopup.hpp"
#include "GetCharacterPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace spine;
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
    
    auto getPopupName = [](PopupType type) -> string {
        switch( type ) {
            case PopupType::NONE:              return "NONE";
            case PopupType::LOADING_BAR:       return "LOADING_BAR";
            case PopupType::EXIT_APP:          return "EXIT_APP";
            case PopupType::REVIEW:            return "REVIEW";
            case PopupType::CREDIT:            return "CREDIT";
            case PopupType::SHOP:              return "SHOP";
            case PopupType::GET_CHARACTER:     return "GET_CHARACTER";
            case PopupType::SETTING:           return "SETTING";
            case PopupType::RANKING:           return "RANKING";
            case PopupType::NEW_RECORD:        return "NEW_RECORD";
            case PopupType::PAUSE:             return "PAUSE";
            case PopupType::GAME_OVER:         return "GAME_OVER";
            case PopupType::CONTINUE:          return "CONTINUE";
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
    CCASSERT(popup->getType() != PopupType::NONE, "PopupManager::addPopup error: type must be not none");
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
BasePopup* PopupManager::getPopup(PopupType type) {
    
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
size_t PopupManager::getPopupCount(PopupType type) {
    
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
        if( popup->getType() == PopupType::RANKING ||
            popup->getType() == PopupType::NEW_RECORD ||
            popup->getType() == PopupType::GAME_OVER ) {
            cnt++;
        }
    }
    
    return cnt;
}

/**
 * 해당 타입의 팝업 존재 여부 반환
 */
bool PopupManager::exists(PopupType type) {
    
    return getPopup(type) != nullptr;
}

/**
 * 팝업 노출
 */
void PopupManager::show(OnPopupEvent onEventListener, PopupType type) {
    
    auto popup = createPopup(type);
    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
        
        if( onEventListener ) {
            onEventListener(sender, eventType);
        }
    });
    SceneManager::getInstance()->getScene()->addChild(popup, ZOrder::POPUP_BOTTOM);
    
    popup->runEnterAction();
}

void PopupManager::show(PopupType type) {

    show(nullptr, type);
}

/**
 * 캐릭터 획득 팝업 노출
 */
void PopupManager::showGetCharacterPopup(const Characters &characters) {
   
    auto &getCharacterPopups = getInstance()->getCharacterPopups;
    
    // 이전 리스트 클리어
    getCharacterPopups.clear();
    
    // firebase event
    {
        for( auto chc : characters ) {
            firebase::Analytics::EventParams params;
            params[FA_EVENT_PARAM_CHAR_ID] = Value(chc.charId);
            
            firebase::Analytics::logEvent(FA_EVENT_CHARACTER_GET, params);
        }
    }
    
    // Effect 노드 생성
    const int POPUP_ZORDER = ZOrder::POPUP_TOP+1;
    const int SHINE_ACTION_TAG_FADE = 100;
    
    // bg
    auto darkBg = LayerColor::create(Color::POPUP_BG);
    SceneManager::getScene()->addChild(darkBg, POPUP_ZORDER-1);
    
    // shine
    auto shine = Sprite::create(DIR_IMG_GAME + "shine_effect.png");
    shine->setAnchorPoint(ANCHOR_M);
    shine->setPosition(Vec2MC(0, 25));
    shine->setScale(4);
    shine->setVisible(false);
    SceneManager::getScene()->addChild(shine, POPUP_ZORDER-1);
    
    // powder
    auto powder = SkeletonAnimation::createWithJsonFile(DIR_ANIM + "get_powder.json");
    powder->setAnchorPoint(Vec2::ZERO);
    powder->setPosition(Vec2MC(0,0));
    powder->setVisible(false);
    SceneManager::getScene()->addChild(powder, POPUP_ZORDER+1);
    
    // 팝업 생성
    for( auto character : characters ) {
        auto popup = GetCharacterPopup::create(character);
        popup->setVisible(false);
        popup->setIgnoreDismissAction(true); // 퇴장 연출 off
        popup->setOnDismissListener([=](Node*) {
    
            // 노출된 팝업 제거
            auto &getCharacterPopups = getInstance()->getCharacterPopups;
            getCharacterPopups.erase(getCharacterPopups.begin());
            
            if( getCharacterPopups.size() == 0 ) {
                // 마지막 팝업 퇴장 완료
                return;
            }
            
            // 다음 팝업 노출
            auto nextPopup = getCharacterPopups[0];
            nextPopup->setVisible(true);
            nextPopup->onEnterActionFinished();
        });
        SceneManager::getScene()->addChild(popup, POPUP_ZORDER);
        
        getCharacterPopups.push_back(popup);
    }
    
    // 첫번째 팝업 노출
    auto firstPopup = getCharacterPopups[0];
    firstPopup->setVisible(true);
    firstPopup->runEnterAction([=]() {
        
        // shine
        shine->setVisible(true);
        shine->setOpacity(0);
        shine->runAction(RepeatForever::create(RotateBy::create(4.0f, 360)));
        
        auto fadeIn = FadeIn::create(0.2f);
        fadeIn->setTag(SHINE_ACTION_TAG_FADE);
        shine->runAction(fadeIn);
        
        // powder
        powder->setVisible(true);
        powder->setAnimation(0, ANIM_NAME_RUN, true);
        
        // 효과음
        SBAudioEngine::playEffect(DIR_SOUND + "fanfare.mp3");
    });
    
    // 배경 등장 연출
    darkBg->setOpacity(0);
    darkBg->runAction(FadeTo::create(0.15f, Color::POPUP_BG.a));
    
    // 마지막 팝업 퇴장 연출 나오도록
    auto lastPopup = getCharacterPopups[getCharacterPopups.size()-1];
    lastPopup->setIgnoreDismissAction(false);
    lastPopup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
        
        switch( eventType ) {
            case PopupEventType::EXIT_ACTION: {
                // bg
                darkBg->stopAllActions();
                darkBg->runAction(Sequence::create(FadeOut::create(0.15f), RemoveSelf::create(), nullptr));
                
                // shine
                shine->stopActionByTag(SHINE_ACTION_TAG_FADE);
                shine->runAction(Sequence::create(FadeOut::create(0.06f), RemoveSelf::create(), nullptr));
                
                // powder
                powder->runAction(Sequence::create(FadeOut::create(0.06f), RemoveSelf::create(), nullptr));
            } break;
                
            case PopupEventType::EXIT_ACTION_FINISHED: {
                
            } break;
                
            default: break;
        }
    });
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
        SceneManager::getScene()->addChild(popup2, ZOrder::POPUP_BOTTOM);
        popup2->runEnterAction();
    });
    
    commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::BACK);
}

void PopupManager::cross(BasePopup *popup1, BasePopup *popup2, BasePopup *popup3) {
    
    cross(nullptr, popup1, popup2, popup3);
}

BasePopup* PopupManager::createPopup(PopupType type) {
    
    switch( type ) {
        case PopupType::SHOP:                  return ShopPopup::create();
        case PopupType::RANKING:               return RankingPopup::create();
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

void PopupManager::removeListener(Ref *target) {
    
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


