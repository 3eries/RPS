//
//  PausePopup.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "PausePopup.hpp"

#include "RSP.h"
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

PausePopup::PausePopup() : BasePopup(Type::PAUSE),
onClickMenuListener(nullptr) {
    
}

PausePopup::~PausePopup() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool PausePopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }

    // IAP 리스너
    {
        auto onRemoveAds = [=]() {
            // vip 마크 표시
            getContentView()->getChildByTag(Tag::REMOVE_ADS)->setVisible(false);
            getContentView()->getChildByTag(Tag::VIP_MARK)->setVisible(true);
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

void PausePopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void PausePopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void PausePopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_pause.png Vec2MC(0, 0) , Size(552, 420)
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_pause.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 0));
    addContentChild(stoneBg);
    
    // title
    // RSP_popup_title_pause.png Vec2MC(4, 141) , Size(292, 64)
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_pause.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 141));
    addContentChild(title);
 
    // 사운드
    // RSP_btn_sound.png Vec2MC(-172, 6) , Size(160, 152)
    auto soundBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_sound_off.png",
                                           DIR_IMG_GAME + "RSP_btn_sound.png");
    soundBtn->setZoomScale(0.07f);
    soundBtn->setAnchorPoint(ANCHOR_M);
    soundBtn->setPosition(Vec2MC(-172, 6));
    addContentChild(soundBtn);
    
    auto audioEngine = SBAudioEngine::getInstance();
    
    soundBtn->setSelected(!audioEngine->isEffectMute() || !audioEngine->isBGMMute());
    soundBtn->setOnSelectedListener([=](bool isSelected) -> bool {
      
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        audioEngine->setMute(!isSelected);
        
        return false;
    });
    
    // 진동
    // RSP_btn_Vibrate.png Vec2MC(0, 10) , Size(160, 152)
    auto vibrateBtn = SBToggleButton::create(DIR_IMG_GAME + "RSP_btn_vibrate_off.png",
                                             DIR_IMG_GAME + "RSP_btn_vibrate.png");
    vibrateBtn->setZoomScale(0.07f);
    vibrateBtn->setAnchorPoint(ANCHOR_M);
    vibrateBtn->setPosition(Vec2MC(0, 10));
    addContentChild(vibrateBtn);
    
    vibrateBtn->setSelected(UserDefault::getInstance()->getBoolForKey(UserDefaultKey::VIBRATE, true));
    vibrateBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        UserDefault::getInstance()->setBoolForKey(UserDefaultKey::VIBRATE, isSelected);
        UserDefault::getInstance()->flush();
        
        return false;
    });
    
    // 기타 메뉴
    {
        // RSP_btn_home.png Vec2MC(172, 10) , Size(160, 152)
        // RSP_btn_remove_ads.png Vec2MC(2, -130) , Size(316, 80)
        SBUIInfo infos[] = {
            SBUIInfo(Tag::MAIN,          ANCHOR_M,   Vec2MC(172, 10),    "RSP_btn_home.png"),
            SBUIInfo(Tag::REMOVE_ADS,    ANCHOR_M,   Vec2MC(0, -130),    "RSP_btn_remove_ads.png"),
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto btn = SBButton::create(DIR_IMG_GAME + info.file);
            btn->setZoomScale(0.07f);
            info.apply(btn);
            addContentChild(btn, info.zOrder, info.tag);
            
            btn->setOnClickListener([=](Node*) {
                
                SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
                this->performListener((Tag)info.tag);
            });
        }
        
        // VIP 마크
        auto vip = UIHelper::createVIPMark(getContentView()->getChildByTag(Tag::REMOVE_ADS));
        vip->setTag(Tag::VIP_MARK);
        vip->setVisible(User::isOwnRemoveAdsItem());
        
        if( User::isOwnRemoveAdsItem() ) {
            getContentView()->getChildByTag(Tag::REMOVE_ADS)->setVisible(false);
        }
    }
}

void PausePopup::performListener(Tag tag) {
    
    retain();
    onClickMenuListener(tag);
    release();
}

/**
 * 등장 연출
 */
void PausePopup::runEnterAction(SBCallback onFinished) {
    
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
void PausePopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(SLIDE_DURATION, onFinished);
    
     // 배경 fade out
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);
    
    // 컨텐츠 slide out
    runSlideOutAction([=]() {
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
    
    // 설정 버튼으로 전환
    SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::PAUSE, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void PausePopup::onEnterActionFinished() {
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->performListener(Tag::RESUME);
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
}
