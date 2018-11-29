//
//  PlayGuidePopup.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 29/11/2018.
//

#include "PlayGuidePopup.hpp"

#include "Define.h"
#include "PopupManager.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"
#include "UserDefaultKey.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

PlayGuidePopup::PlayGuidePopup() : BasePopup(PopupType::PLAY_GUIDE) {
}

PlayGuidePopup::~PlayGuidePopup() {
}

bool PlayGuidePopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    // 화면에 가이드 팝업이 유일할때만 배경 생성
    if( PopupManager::getPopupCount() == 1 ) {
        setBackgroundColor(Color::POPUP_BG);
    }
    
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_how_to_play.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0, 50));
    addChild(bg);
    
    auto okBtn = SBButton::create(DIR_IMG_GAME + "btn_ok.png");
    okBtn->setAnchorPoint(ANCHOR_M);
    okBtn->setPosition(Vec2MC(bg->getContentSize(), 0, -357));
    bg->addChild(okBtn);
    
    okBtn->setOnClickListener([=](Node*) {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        // show top menu
        SceneManager::getCommonMenu()->getTopMenu()->setVisible(true);
        
        // dismiss
        this->dismiss();
    });
    
    return true;
}

void PlayGuidePopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

/**
 * 등장 연출
 */
void PlayGuidePopup::runEnterAction(SBCallback onFinished) {
    
    const float DURATION = 0.2f;
    
    BasePopup::runEnterAction(DURATION, onFinished);
    
    SBNodeUtils::recursiveCascadeOpacityEnabled(this, true);
    
    // fade in
    setOpacity(0);
    
    auto fadeIn = FadeIn::create(DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        this->retain();
        
        if( onFinished ) {
            onFinished();
        }
        
        this->onEnterActionFinished();
        this->release();
    });
    runAction(Sequence::create(fadeIn, callFunc, nullptr));
    
    // hide top menu
    SceneManager::getCommonMenu()->getTopMenu()->setVisible(false);
}

/**
 * 등장 연출 완료
 */
void PlayGuidePopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
