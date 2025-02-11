//
//  SettingPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#include "SettingPopup.hpp"

#include "Define.h"
#include "User.hpp"
#include "UserDefaultKey.h"

#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = EffectDuration::POPUP_SLIDE_FAST;

SettingPopup::SettingPopup() : BasePopup(PopupType::SETTING),
onClickMenuListener(nullptr) {
    
}

SettingPopup::~SettingPopup() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool SettingPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    // IAP 리스너
    {
        auto onRemoveAds = [=]() {
            // vip 마크 표시
            stoneBg->getChildByTag(Tag::REMOVE_ADS)->setVisible(false);
            stoneBg->getChildByTag(Tag::VIP_MARK)->setVisible(true);
        };
        
        // purchase listener
        auto purchaseListener = iap::PurchaseListener::create();
        purchaseListener->setForever(true);
        purchaseListener->onRemoveAds = onRemoveAds;
        
        iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
        
        // restore listener
        auto restoreListener = iap::RestoreListener::create();
        restoreListener->setForever(true);
        restoreListener->onRemoveAds = onRemoveAds;
        
        iap::IAPHelper::getInstance()->addListener(this, restoreListener);
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
    
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_settings.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 60));
    addContentChild(stoneBg);
    
    const Size bgSize(stoneBg->getContentSize());
    
    // title
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_settings.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(bgSize, 0, 256));
    stoneBg->addChild(title);
    
    // 효과음
    auto effectBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_effect_off.png",
                                            DIR_IMG_GAME + "RSP_btn_effect.png");
    effectBtn->setZoomScale(0.07f);
    effectBtn->setAnchorPoint(ANCHOR_M);
    effectBtn->setPosition(Vec2MC(bgSize, -172, 124));
    stoneBg->addChild(effectBtn);
    
    auto audioEngine = SBAudioEngine::getInstance();
    
    effectBtn->setSelected(!audioEngine->isEffectMute());
    effectBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        audioEngine->setEffectMute(!isSelected);
        
        return false;
    });
    
    // 배경음
    auto bgmBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_bgm_off.png",
                                         DIR_IMG_GAME + "RSP_btn_bgm.png");
    bgmBtn->setZoomScale(0.07f);
    bgmBtn->setAnchorPoint(ANCHOR_M);
    bgmBtn->setPosition(Vec2MC(bgSize, 0, 124));
    stoneBg->addChild(bgmBtn);
    
    bgmBtn->setSelected(!audioEngine->isBGMMute());
    bgmBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        audioEngine->setBGMMute(!isSelected);
        
        return false;
    });
    
    // 진동
    auto vibrateBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_vibrate_off.png",
                                             DIR_IMG_GAME + "RSP_btn_vibrate.png");
    vibrateBtn->setZoomScale(0.07f);
    vibrateBtn->setAnchorPoint(ANCHOR_M);
    vibrateBtn->setPosition(Vec2MC(bgSize, 172, 124));
    stoneBg->addChild(vibrateBtn);
    
    vibrateBtn->setSelected(UserDefault::getInstance()->getBoolForKey(UserDefaultKey::VIBRATE, true));
    vibrateBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        UserDefault::getInstance()->setBoolForKey(UserDefaultKey::VIBRATE, isSelected);
        UserDefault::getInstance()->flush();
        
        return false;
    });
    
    // 기타 메뉴
    {
        SBUIInfo infos[] = {
            SBUIInfo(Tag::WORLD_RANKING,     ANCHOR_M,   Vec2MC(bgSize, -88, -42),   "RSP_btn_ranking_world.png"),
            SBUIInfo(Tag::RESTORE_PURCHASE,  ANCHOR_M,   Vec2MC(bgSize, 88, -42),    "RSP_btn_restore.png"),
            SBUIInfo(Tag::REMOVE_ADS,        ANCHOR_M,   Vec2MC(bgSize, 0, -171),    "RSP_btn_remove_ads.png"),
            SBUIInfo(Tag::HOW_TO_PLAY,       ANCHOR_M,   Vec2MC(bgSize, 0, -259),    "RSP_how_to_play.png"),
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto btn = SBButton::create(DIR_IMG_GAME + info.file);
            btn->setZoomScale(0.07f);
            info.apply(btn);
            stoneBg->addChild(btn);
            
            btn->setOnClickListener([=](Node*) {
                
                SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
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
    
    BasePopup::runEnterAction(SLIDE_DURATION, onFinished);
    
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
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
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
    
    BasePopup::onEnterActionFinished();
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
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
