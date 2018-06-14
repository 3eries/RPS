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
}

void MainScene::onEnterTransitionDidFinish() {
    
    Scene::onEnterTransitionDidFinish();
    
    SBAudioEngine::playBGM(SOUND_BGM_MAIN);
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
        case Tag::BTN_TITLE: {
            // TODO: 크레딧
        } break;
            
        case Tag::BTN_START: {
            replaceGame();
        } break;
            
        case Tag::BTN_REMOVE_ADS: {
            User::setOwnRemoveAdsItem(!User::isOwnRemoveAdsItem());
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
        SBUIInfo(Tag::BTN_START, ANCHOR_MB, Vec2BC(0, 120), ""),
        SBUIInfo(Tag::BTN_REMOVE_ADS, ANCHOR_M, Vec2MC(0, 0),    ""),
    };
    
    string titles[] = {
        "START",
        "REMOVE ADS",
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto btn = UIHelper::createFontButton(titles[i], ButtonSize::MEDIUM);
        infos[i].apply(btn);
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
    
    // test
    {
        auto btn = UIHelper::createFontButton("TEST", ButtonSize::MEDIUM);
        btn->setAnchorPoint(ANCHOR_MB);
        btn->setPosition(Vec2BC(0, 10));
        addChild(btn);
        
        btn->setOnClickListener([=](Node*) {
            Director::getInstance()->pushScene(TestMenuScene::create());
        });
    }
}
