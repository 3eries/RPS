//
//  GetCharacterPopup.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#include "GetCharacterPopup.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

#include "../game/GameDefine.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

GetCharacterPopup* GetCharacterPopup::create(Character character) {
    
    auto popup = new GetCharacterPopup(character);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    delete popup;
    return nullptr;
}

GetCharacterPopup::GetCharacterPopup(Character character) : BasePopup(PopupType::GET_CHARACTER),
character(character) {
    
}

GetCharacterPopup::~GetCharacterPopup() {
    
}

bool GetCharacterPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void GetCharacterPopup::onEnter() {
    
    BasePopup::onEnter();
}

void GetCharacterPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void GetCharacterPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_get.png Vec2MC(0, 0) , Size(552, 684)
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_get.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 25));
    addContentChild(stoneBg);
    
    const Size bgSize(stoneBg->getContentSize());
    
    // title
    // RSP_popup_title_get.png Vec2TC(0, -64) , Size(256, 72)
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_get.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(bgSize, 0, -64));
    stoneBg->addChild(title);
    
    // 캐릭터 이름
    // RSP_popup_text_zeus.png Vec2MC(0, 131) , Size(486, 218)
    string nameFile = DIR_ADD(DIR_CHARACTER, character.packId) + character.charId + "_name.png";
    
    auto characterNameImage = Sprite::create(nameFile);
    characterNameImage->setAnchorPoint(ANCHOR_M);
    characterNameImage->setPosition(Vec2MC(bgSize, 0, 131));
    stoneBg->addChild(characterNameImage);
    
    // 캐릭터 이미지
    // RSP_zeus_idle1.png Vec2MC(39, -26) , Size(388, 296)
    auto anim = SBNodeUtils::createAnimation(character.idleAnims, HERO_ANIM_IDLE_DELAY_PER_UNIT);
    
    auto characterImage = SBAnimationSprite::create(anim);
    characterImage->setAnchorPoint(ANCHOR_M);
    characterImage->setPosition(Vec2MC(bgSize, 39, -26));
    stoneBg->addChild(characterImage);

    characterImage->runAnimation();
    
    // OK 버튼
    // btn_ok.png Vec2BC(0, 87) , Size(400, 112)
    auto okBtn = SBButton::create(DIR_IMG_GAME + "btn_ok.png");
    okBtn->setZoomScale(0.07f);
    okBtn->setAnchorPoint(ANCHOR_M);
    okBtn->setPosition(Vec2BC(bgSize, 0, 87));
    stoneBg->addChild(okBtn);
    
    okBtn->setOnClickListener([=](Node*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        this->dismissWithAction();
    });
}

/**
 * 등장 연출
 */
void GetCharacterPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide in
    runSlideInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 퇴장 연출
 */
void GetCharacterPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void GetCharacterPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        // this->dismissWithAction();
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
    
    // stoneTouchNode->addChild(SBNodeUtils::createBackgroundNode(stoneTouchNode, Color4B::RED), -1);
}


