//
//  BasePopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 5..
//

#include "BasePopup.hpp"

#include "RSP.h"
#include "PopupManager.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

BasePopup::BasePopup(Type type) : SBBasePopup(),
type(type),
popupMgr(PopupManager::getInstance()),
onPopupEventListener(nullptr),
enterTimeScale(1),
exitTimeScale(1),
runningEnterAction(false),
runningExitAction(false) {
}

BasePopup::~BasePopup() {
    
}

bool BasePopup::init() {
    
    if( !SBBasePopup::init() ) {
        return false;
    }
    
    popupMgr->addPopup(this);
    
    // back key
    {
        auto listener = EventListenerKeyboard::create();
        listener->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event *event) {
            
            if( keyCode != EventKeyboard::KeyCode::KEY_BACK ) {
                return;
            }
            
            if( SceneManager::getInstance()->onBackKeyReleased() ) {
                return;
            }
            
            // 광고가 열렸다 닫힌 경우 예외처리
            if( !Director::getInstance()->isValid() ) {
                return;
            }
            
            // 등/퇴장 연출중
            if( runningEnterAction || runningExitAction ) {
                return;
            }
            
            // 우상단 닫기 버튼 활성화된 경우, 닫기 버튼 클릭 강제 수행
            auto commonMenu = SceneManager::getCommonMenu();
            if( !commonMenu ) {
                return;
            }
            
            auto topMenu = commonMenu->getTopMenu();
            if( !topMenu->isVisible() ) {
                return;
            }
            
            auto selectedMenu = topMenu->getSelectedRightMenu();
            
            if( selectedMenu== TopMenu::Tag::BACK ||
                selectedMenu == TopMenu::Tag::CLOSE ) {
                
                commonMenu->performClickTopMenu(selectedMenu);
            }
        };
        
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    return true;
}

void BasePopup::onEnter() {
    
    SBBasePopup::onEnter();
    
    onPopupEvent(PopupEventType::ENTER);
}

void BasePopup::onExit() {
    
    onPopupEvent(PopupEventType::EXIT);
    popupMgr->removePopup(this);
    
    getEventDispatcher()->removeEventListenersForTarget(this);
    
    SBBasePopup::onExit();
}

void BasePopup::onPopupEvent(PopupEventType eventType) {
    
    popupMgr->dispatchEvent(this, eventType);
    
    if( onPopupEventListener ) {
        onPopupEventListener(this, eventType);
    }
}

void BasePopup::setBackgroundColor(const Color4B &color) {
    
    auto bgView = getBackgroundView();
    bgView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    bgView->setBackGroundColor(Color3B(color));
    bgView->setBackGroundColorOpacity(color.a);
}

void BasePopup::dismissWithAction(SBCallback onFinished) {
    
    runExitAction([=]() {
        
        this->retain();
        
        if( onFinished ) {
            onFinished();
        }
        
        this->dismiss();
        this->release();
    });
}

/**
 * 등장 연출 시작
 */
void BasePopup::runEnterAction(float duration, SBCallback onFinished) {
    
    runningEnterAction = true;
    onPopupEvent(PopupEventType::ENTER_ACTION);
    
    savedTopMenu = SceneManager::getCommonMenu()->getTopMenu()->getSelectedRightMenu();
}

void BasePopup::runEnterAction(SBCallback onFinished) {

    runEnterAction(0, onFinished);
}

/**
 * 퇴장 연출 시작
 */
void BasePopup::runExitAction(float duration, SBCallback onFinished) {
    
    runningExitAction = true;
    onPopupEvent(PopupEventType::EXIT_ACTION);
}

void BasePopup::runExitAction(SBCallback onFinished) {
    
    runExitAction(0, onFinished);
}

/**
 * 등장 연출 완료
 */
void BasePopup::onEnterActionFinished() {
   
    runningEnterAction = false;
    onPopupEvent(PopupEventType::ENTER_ACTION_FINISHED);
}

/**
 * 퇴장 연출 완료
 */
void BasePopup::onExitActionFinished() {
    
    runningExitAction = false;
    onPopupEvent(PopupEventType::EXIT_ACTION_FINISHED);
}

/**
 * slide
 */
void BasePopup::runSlideAction(SBCallback onFinished, float duratoin, Vec2 from, Vec2 to) {
    
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    contentView->setPosition(from);
    
    auto move = MoveTo::create(duratoin, to);
    auto callFunc = CallFunc::create([=]() {
        
        SBDirector::getInstance()->setScreenTouchLocked(false);
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    contentView->runAction(Sequence::create(move, callFunc, nullptr));
}

/**
 * slide in
 */
void BasePopup::runSlideInAction(SBCallback onFinished, float duratoin) {
    
    runSlideAction(onFinished, duratoin, Vec2(0, SB_WIN_SIZE.height), Vec2(0, 0));
}

/**
 * slide out
 */
void BasePopup::runSlideOutAction(SBCallback onFinished, float duratoin) {

    runSlideAction(onFinished, duratoin, Vec2(0, 0), Vec2(0, SB_WIN_SIZE.height));
}
