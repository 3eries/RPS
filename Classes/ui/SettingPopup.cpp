//
//  SettingPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#include "SettingPopup.hpp"

#include "RSP.h"
#include "User.hpp"
#include "UserDefaultKey.h"

#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

SettingPopup::SettingPopup() : BasePopup(Type::SETTING),
onClickMenuListener(nullptr) {
    
}

SettingPopup::~SettingPopup() {
    
    superbomb::IAPHelper::getInstance()->removeListener(this);
}

bool SettingPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    // IAP 리스너
    {
        auto listener = superbomb::IAPListener::create();
        listener->onRemoveAdsPurchased = [=](const superbomb::Product &prod) {
            
            // vip 마크 표시
            stoneBg->getChildByTag(Tag::REMOVE_ADS)->setVisible(false);
            stoneBg->getChildByTag(Tag::VIP_MARK)->setVisible(true);
        };
        
        superbomb::IAPHelper::getInstance()->addListener(this, listener);
    }
    
    return true;
}

void SettingPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void SettingPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void SettingPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_settings.png Vec2MC(0, 60) , Size(552, 616)
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_settings.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 60));
    addContentChild(stoneBg);
    
    const Size bgSize(stoneBg->getContentSize());
    
    // title
    // RSP_popup_title_settings.png Vec2MC(0, 236) , Size(344, 56)
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_settings.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(bgSize, 0, 236));
    stoneBg->addChild(title);
    
    // 효과음
    // RSP_btn_effect.png Vec2MC(-172, 92) , Size(160, 152)
    auto effectBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_effect_off.png",
                                            DIR_IMG_GAME + "RSP_btn_effect.png");
    effectBtn->setZoomScale(0.07f);
    effectBtn->setAnchorPoint(ANCHOR_M);
    effectBtn->setPosition(Vec2MC(bgSize, -172, 92));
    stoneBg->addChild(effectBtn);
    
    auto audioEngine = SBAudioEngine::getInstance();
    
    effectBtn->setSelected(!audioEngine->isEffectMute());
    effectBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        audioEngine->setEffectMute(!isSelected);
        
        return false;
    });
    
    // 배경음
    // RSP_btn_bgm.png Vec2MC(0, 92) , Size(160, 152)
    auto bgmBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_bgm_off.png",
                                         DIR_IMG_GAME + "RSP_btn_bgm.png");
    bgmBtn->setZoomScale(0.07f);
    bgmBtn->setAnchorPoint(ANCHOR_M);
    bgmBtn->setPosition(Vec2MC(bgSize, 0, 92));
    stoneBg->addChild(bgmBtn);
    
    bgmBtn->setSelected(!audioEngine->isBGMMute());
    bgmBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        audioEngine->setBGMMute(!isSelected);
        
        return false;
    });
    
    // 진동
    // RSP_btn_vibrate.png Vec2MC(172, 96) , Size(160, 152)
    auto vibrateBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_vibrate_off.png",
                                             DIR_IMG_GAME + "RSP_btn_vibrate.png");
    vibrateBtn->setZoomScale(0.07f);
    vibrateBtn->setAnchorPoint(ANCHOR_M);
    vibrateBtn->setPosition(Vec2MC(bgSize, 172, 96));
    stoneBg->addChild(vibrateBtn);
    
    vibrateBtn->setSelected(UserDefault::getInstance()->getBoolForKey(UserDefaultKey::VIBRATE, true));
    vibrateBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        UserDefault::getInstance()->setBoolForKey(UserDefaultKey::VIBRATE, isSelected);
        UserDefault::getInstance()->flush();
        
        return false;
    });
    
    // 기타 메뉴
    {
        // RSP_btn_ranking_world.png Vec2MC(-88, -80) , Size(160, 152)
        // RSP_btn_restore.png Vec2MC(88, -80) , Size(160, 152)
        // RSP_btn_remove_ads.png Vec2MC(-2, -216) , Size(316, 80)
        SBUIInfo infos[] = {
            SBUIInfo(Tag::WORLD_RANKING,     ANCHOR_M,   Vec2MC(bgSize, -88, -80),   "RSP_btn_ranking_world.png"),
            SBUIInfo(Tag::RESTORE_PURCHASE,  ANCHOR_M,   Vec2MC(bgSize, 88, -80),    "RSP_btn_restore.png"),
            SBUIInfo(Tag::REMOVE_ADS,        ANCHOR_M,   Vec2MC(bgSize, 0, -216),    "RSP_btn_remove_ads.png"),
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto btn = SBButton::create(DIR_IMG_GAME + info.file);
            btn->setZoomScale(0.07f);
            info.apply(btn);
            stoneBg->addChild(btn);
            
            btn->setOnClickListener([=](Node*) {
                this->performListener((Tag)info.tag);
            });
        }
        
        // VIP 마크
        auto vip = UIHelper::createVIPMark(stoneBg->getChildByTag(Tag::REMOVE_ADS));
        vip->setTag(Tag::VIP_MARK);
        vip->setVisible(User::isOwnRemoveAdsItem());
        
        if( User::isOwnRemoveAdsItem() ) {
            stoneBg->getChildByTag(Tag::REMOVE_ADS)->setVisible(false);
        }
    }
}

void SettingPopup::performListener(Tag tag) {
    
    retain();
    onClickMenuListener(tag);
    release();
}

/**
 * 등장 연출
 */
void SettingPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(onFinished);
    
    // 배경 fade in
    runBackgroundFadeInAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide in
    runSlideInAction([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
    
    // 닫기 버튼으로 전환
    SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::BACK, SLIDE_DURATION);
}

/**
 * 퇴장 연출
 */
void SettingPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(onFinished);
    
    // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
    
    // 설정 버튼으로 전환
    SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void SettingPopup::onEnterActionFinished() {
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->dismissWithAction();
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
    
    // stoneTouchNode->addChild(SBNodeUtils::createBackgroundNode(stoneTouchNode, Color4B::RED), -1);
}
