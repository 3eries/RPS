//
//  MainScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "MainScene.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "UIHelper.hpp"

#include "CommonLoadingBar.hpp"
#include "CreditPopup.hpp"
#include "RankingPopup.hpp"
#include "ExitAlertPopup.hpp"

#include "../test/TestHelper.hpp"
#include "../test/TestMenuScene.hpp"

#include "../game/GameView.hpp"
#include "../game/object/Man.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

MainScene::MainScene() {
    
}

MainScene::~MainScene() {
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool MainScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    firebase::Analytics::setScreenName(FA_SCREEN_MAIN);
    
    initBg();
    initMenu();
    initCommonMenu();
    
    initIAPListener();
    
    return true;
}

void MainScene::onEnter() {
    
    BaseScene::onEnter();
    
    initPopupListener();
    
    // bgm
    scheduleOnce([=](float) {
        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(true);
    }
}

void MainScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        ConsentForm::create()
        ->setTitle("Important")
        ->setMessage("We don't collect your personal data,\nhowever our ads suppliers may use\nthem to personalise ads for you. You'll\nfind links to our partner's privacy\npolicies in our Privacy Policy.\n\nYou can opt-out ads tracking from\ngame & device settings.")
        ->setPolicyUrl("http://www.3eries.com/privacy_policy")
        ->show();
    }
    
    // 패키지 DB 업데이트, DAILY_LOGIN
    if( SBDirector::isTodayFirstRun() ) {
        CharacterListener listener;
        listener.onCharacterUnlocked = [=](Characters characters) {
            PopupManager::showGetCharacterPopup(characters);
        };
        
        auto charMgr = CharacterManager::getInstance();
        charMgr->submit(listener, PackageDB::Field::DAILY_LOGIN);
        charMgr->commitAll();
    }
}

void MainScene::onExit() {
    
    PopupManager::getInstance()->removeListener(this);
    
    BaseScene::onExit();
}

/**
 * 게임 화면으로 이동
 */
void MainScene::replaceGame() {
    
    SceneManager::getInstance()->replace(SceneType::GAME);
}

/**
 * 버튼 클릭
 */
void MainScene::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // 크레딧
        case Tag::BTN_TITLE: {        
            firebase::Analytics::logEvent(FA_EVENT_CREDIT);
            
            auto popup = CreditPopup::create();
            addChild(popup, ZOrder::POPUP_TOP);
        } break;
        
        // 광고 제거 아이템
        case Tag::BTN_REMOVE_ADS: {
            if( iap::IAPHelper::isReady() ) {
                auto loadingBar = CommonLoadingBar::create();
                loadingBar->setUIDelay(0.1f);
                loadingBar->show();
                
                auto listener = iap::PurchaseListener::create();
                listener->setTarget(this);
                listener->onPurchased = [=](const iap::Item &item) {
                };
                listener->onFinished = [=](bool result) {
                    loadingBar->dismissWithDelay(0);
                };

                iap::IAPHelper::purchaseRemoveAds(listener);
            }
            
        } break;
            
        // test
        case Tag::BTN_TEST: {
            Director::getInstance()->pushScene(TestMenuScene::create());
        } break;
            
        default:
            break;
    }
}

void MainScene::initBg() {
    
    contentView = SBNodeUtils::createZeroSizeNode();
    addChild(contentView);
    
    auto title = SBButton::create(DIR_IMG_GAME + "RSP_title.png");
    title->setTag(Tag::BTN_TITLE);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 255));
    title->setZoomScale(0);
    contentView->addChild(title);
    
    title->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
}

/**
 * 메뉴 초기화
 */
void MainScene::initMenu() {
    
    // 메인 화면 전용 메뉴
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_REMOVE_ADS,   ANCHOR_M,    Vec2MC(0, 25),     "RSP_btn_remove_ads.png"),
#if ENABLE_TEST_MENU
        SBUIInfo(Tag::BTN_TEST,         ANCHOR_TL,   Vec2TL(10, -10),   "RSP_btn_test.png"),
#endif
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        btn->setZoomScale(0.1f);
        info.apply(btn);
        contentView->addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
    }
    
    // VIP 마크
    auto vip = UIHelper::createVIPMark(contentView->getChildByTag(Tag::BTN_REMOVE_ADS));
    vip->setTag(Tag::VIP_MARK);
    vip->setVisible(User::isOwnRemoveAdsItem());
    
    if( User::isOwnRemoveAdsItem() ) {
        contentView->getChildByTag(Tag::BTN_REMOVE_ADS)->setVisible(false);
    }
}

/**
 * 인앱 결제 리스너 초기화
 */
void MainScene::initIAPListener() {
    
    auto onRemoveAds = [=]() {
        // vip 마크 표시
        contentView->getChildByTag(Tag::BTN_REMOVE_ADS)->setVisible(false);
        contentView->getChildByTag(Tag::VIP_MARK)->setVisible(true);
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

/**
 * 팝업 리스너 초기화
 */
void MainScene::initPopupListener() {
    
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType type) {
        
        auto popup = dynamic_cast<BasePopup*>(sender);
        
        // 팝업 연출에 따른 메인화면 메뉴 처리
        if( popup->getType() != PopupType::RANKING &&
            popup->getType() != PopupType::SHOP) {
            return;
        }
        
        if( PopupManager::getPopup(PopupType::RANKING) &&
            PopupManager::getPopup(PopupType::SHOP) ) {
            return;
        }
        
        auto man = SceneManager::getGameView()->getMan();
        
        switch( type ) {
            case PopupEventType::ENTER_ACTION: {
                contentView->runAction(MoveTo::create(RankingPopup::SLIDE_IN_DURATION*1.5f,
                                                      Vec2(0, SB_WIN_SIZE.height)));
            } break;
                
            case PopupEventType::ENTER_ACTION_FINISHED: {
                man->setVisible(false);
            } break;
                
            case PopupEventType::EXIT_ACTION: {
                contentView->runAction(MoveTo::create(RankingPopup::SLIDE_OUT_DURATION,
                                                      Vec2(0, 0)));
                man->setVisible(true);
            } break;
                
            default: break;
        }
    };
    
    PopupManager::getInstance()->addListener(listener);
}

void MainScene::processBackKey() {
    
    auto listener = EventListenerKeyboard::create();
    listener->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event *event) {
        
        if( keyCode != EventKeyboard::KeyCode::KEY_BACK ) {
            return;
        }
        
        if( SceneManager::getInstance()->onBackKeyReleased() ) {
            return;
        }
        
        // 앱 종료 알림 팝업 생성
        if( PopupManager::getInstance()->getPopupCount() == 0 ) {
            SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
            
            auto popup = ExitAlertPopup::create();
            popup->setOnExitAppListener([=]() {
                SBSystemUtils::exitApp();
            });
            
            this->addChild(popup, ZOrder::POPUP_MIDDLE);
        }
    };
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}
