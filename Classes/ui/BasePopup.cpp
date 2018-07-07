//
//  BasePopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 5..
//

#include "BasePopup.hpp"

#include "RSP.h"
#include "PopupManager.hpp"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

BasePopup::BasePopup(Type type) : SBBasePopup(),
type(type),
popupMgr(PopupManager::getInstance()) {
    
}

BasePopup::~BasePopup() {
    
}

bool BasePopup::init() {
    
    if( !SBBasePopup::init() ) {
        return false;
    }
    
    popupMgr->addPopup(this);
    
    return true;
}

void BasePopup::onEnter() {
    
    SBBasePopup::onEnter();
}

void BasePopup::onExit() {
    
    popupMgr->removePopup(this);
    
    SBBasePopup::onExit();
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
void BasePopup::runEnterAction(SBCallback onFinished) {
    
    popupMgr->dispatchEvent(this, PopupEventType::ENTER_ACTION);
}

/**
 * 퇴장 연출 시작
 */
void BasePopup::runExitAction(SBCallback onFinished) {
    
    popupMgr->dispatchEvent(this, PopupEventType::EXIT_ACTION);
}

/**
 * 등장 연출 완료
 */
void BasePopup::onEnterActionFinished() {
    
    popupMgr->dispatchEvent(this, PopupEventType::ENTER_ACTION_FINISHED);
}

/**
 * 퇴장 연출 완료
 */
void BasePopup::onExitActionFinished() {
    
    popupMgr->dispatchEvent(this, PopupEventType::EXIT_ACTION_FINISHED);
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
