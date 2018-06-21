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
#include "UIHelper.hpp"

#include "../test/TestMenuScene.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

MainScene::MainScene() {
    
}

MainScene::~MainScene() {
    
}

bool MainScene::init() {
    
    if( !Scene::init() ) {
        return false;
    }
    
    // back key
    {
        auto listener = EventListenerKeyboard::create();
        listener->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event *event) {
            
            if( keyCode != EventKeyboard::KeyCode::KEY_BACK ) {
                return;
            }
            
            SBSystemUtils::exitApp();
        };
        
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    initBg();
    initMenu();
    
    return true;
}

void MainScene::onEnter() {
    
    Scene::onEnter();
    
    // bgm
    scheduleOnce([=](float) {
        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
}

void MainScene::onEnterTransitionDidFinish() {
    
    Scene::onEnterTransitionDidFinish();
}

void MainScene::onExit() {
    
    Scene::onExit();
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
        case Tag::BTN_START: {
            replaceGame();
        } break;
            
        case Tag::BTN_TITLE: {
            // TODO: 크레딧
        } break;
            
        case Tag::BTN_REMOVE_ADS: {
            User::setOwnRemoveAdsItem(!User::isOwnRemoveAdsItem());
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
    
    auto title = SBButton::create(DIR_IMG_GAME + "RSP_title.png");
    title->setTag(Tag::BTN_TITLE);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 255));
    title->setZoomScale(0);
    addChild(title);
    
    title->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
}

/**
 * 메뉴 초기화
 */
void MainScene::initMenu() {
    
    SBUIInfo infos[] = {
        SBUIInfo(Tag::BTN_START,        ANCHOR_MB,   Vec2BC(0, 20),     "RSP_btn_start.png"),
        SBUIInfo(Tag::BTN_REMOVE_ADS,   ANCHOR_M,    Vec2MC(0, 25),     "RSP_btn_remove_ads.png"),
        SBUIInfo(Tag::BTN_LEADER_BOARD, ANCHOR_BL,   Vec2BL(10, 20),    "RSP_btn_ranking.png"),
        SBUIInfo(Tag::BTN_SHOP,         ANCHOR_BR,   Vec2BR(-10, 20),   "RSP_btn_shop.png"),
        SBUIInfo(Tag::BTN_OPTION,       ANCHOR_TR,   Vec2TR(-10, -10),  "RSP_btn_option.png"),
        SBUIInfo(Tag::BTN_TEST,         ANCHOR_TL,   Vec2TL(10, -10),   "RSP_btn_test.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        btn->setZoomScale(0.1f);
        info.apply(btn);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
    }
    
    // START 버튼 연출
    /*
     auto fadeOut = FadeOut::create(0.2f);
     auto callFuncN = CallFuncN::create([=](Node *sender) {
     
     auto fadeIn = FadeIn::create(0.7f);
     auto fadeOut = FadeOut::create(0.1f);
     //        auto scale1 = ScaleTo::create(0.1, 0);
     //        auto scale2 = ScaleTo::create(0.7f, ICON_SCALE * 1.05f);
     
     auto seq = Sequence::create(fadeIn, fadeOut, nullptr);
     sender->runAction(RepeatForever::create(seq));
     });
     
     getChildByTag(Tag::BTN_START)->runAction(Sequence::create(fadeOut, callFuncN, nullptr));
     */
}
