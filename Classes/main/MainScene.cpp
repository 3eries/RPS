//
//  MainScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "MainScene.hpp"

#include "RSP.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "UIHelper.hpp"

#include "CreditPopup.hpp"
#include "RankingPopup.hpp"
#include "ExitAlertPopup.hpp"

#include "../test/TestMenuScene.hpp"

#include "../game/GameView.hpp"
#include "../game/object/Man.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

MainScene::MainScene() {
    
}

MainScene::~MainScene() {
    
    superbomb::IAPHelper::getInstance()->removeListener(this);
}

bool MainScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
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
            
            // 앱 종료 알림 팝업 처리
            auto popup = PopupManager::getInstance()->getPopup(BasePopup::Type::EXIT_APP);
            
            // 팝업 제거
            if( popup ) {
                popup->dismissWithAction();
            }
            // 팝업 생성
            else if( PopupManager::getInstance()->getPopupCount() == 0 ) {
                auto popup = ExitAlertPopup::create();
                popup->setOnExitAppListener([=]() {
                    SBSystemUtils::exitApp();
                });
                
                this->addChild(popup, PopupZOrder::MIDDLE);
                
                // 배너 노출
                superbomb::AdsHelper::showBanner();
                
                // 팝업 삭제 리스너
                popup->setOnDismissListener([=](Node*) {
                    // 배너 숨김
                    superbomb::AdsHelper::hideBanner();
                });
            }
        };
        
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    // IAP 리스너
    {
        auto listener = superbomb::IAPListener::create();
        listener->onRemoveAdsPurchased = [=](const superbomb::Product &prod) {
            
            // vip 마크 표시
            contentView->getChildByTag(Tag::BTN_REMOVE_ADS)->setVisible(false);
            contentView->getChildByTag(Tag::VIP_MARK)->setVisible(true);
        };
        
        superbomb::IAPHelper::getInstance()->addListener(this, listener);
    }
    
    initBg();
    initMenu();
    initCommonMenu();
    
    return true;
}

void MainScene::onEnter() {
    
    BaseScene::onEnter();
    
    addPopupListener();
    
    // bgm
    scheduleOnce([=](float) {
        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
}

void MainScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
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
    
    switch( sender->getTag() ) {
        // 크레딧
        case Tag::BTN_TITLE: {
            superbomb::firebase::Analytics::logEvent(FA_EVENT_CREDIT);
            
            auto popup = CreditPopup::create();
            addChild(popup, PopupZOrder::TOP);
        } break;
        
        // 광고 제거 아이템
        case Tag::BTN_REMOVE_ADS: {
            // User::setOwnRemoveAdsItem(!User::isOwnRemoveAdsItem());
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
        SBUIInfo(Tag::BTN_TEST,         ANCHOR_TL,   Vec2TL(10, -10),   "RSP_btn_test.png"),
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

void MainScene::addPopupListener() {
    
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType type) {
        
        auto popup = dynamic_cast<BasePopup*>(sender);
        
        // 랭킹 팝업 연출에 따른 메인화면 메뉴 처리
        if( popup->getType() != BasePopup::Type::RANKING ) {
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
