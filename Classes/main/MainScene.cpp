//
//  MainScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "MainScene.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

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
void MainScene::replaceGameScene() {
    
    SceneManager::getInstance()->replaceScene(SceneType::GAME);
}

/**
 * 버튼 클릭
 */
void MainScene::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        case Tag::BTN_START: {
            replaceGameScene();
        } break;
    }
}

/**
 * 배경 초기화
 */
void MainScene::initBg() {
    
    // bg
    addChild(LayerColor::create(Color4B::BLACK));
    
    auto bg = Sprite::create(DIR_IMG_MAIN + "main_bg.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg);
    
    // title
    /*
    auto titleLabel = Label::createWithTTF("ROCK SCISSORS PAPER", FONT_RETRO, 40);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(0, 200));
    addChild(titleLabel);
    */
    string strs[] = {
        "ROCK", "SCISSORS",  "PAPER",
    };
    
    const float ORIGIN_Y = 310;
    const float PADDING_Y = 80;
    
    Vec2 pos[] = {
        Vec2MC(-200, ORIGIN_Y + PADDING_Y), Vec2MC(0, ORIGIN_Y), Vec2MC(200, ORIGIN_Y - PADDING_Y)
    };
    
    for( int i = 0; i < sizeof(strs)/sizeof(string); ++i ) {
        auto titleLabel = Label::createWithTTF(strs[i], FONT_RETRO, 60);
        titleLabel->setAnchorPoint(ANCHOR_M);
        titleLabel->setPosition(pos[i]);
        titleLabel->setColor(Color3B::WHITE);
        titleLabel->enableOutline(Color4B::BLACK, 3);
        addChild(titleLabel);
    }
}

/**
 * 메뉴 초기화
 */
void MainScene::initMenu() {

//    auto btn = SBNodeUtils::createTouchNode();
    auto btn = UIHelper::createFontButton("START", ButtonSize::MEDIUM);
    btn->setTag(Tag::BTN_START);
    btn->setAnchorPoint(ANCHOR_M);
    btn->setPosition(Vec2MC(0, -150));
    addChild(btn);
    
    btn->setOnClickListener(CC_CALLBACK_1(MainScene::onClick, this));
    
    // 연출
    auto fadeOut = FadeOut::create(0.2f);
    auto callFuncN = CallFuncN::create([=](Node *sender) {
        
        auto fadeIn = FadeIn::create(0.7f);
        auto fadeOut = FadeOut::create(0.1f);
        //                auto scale1 = ScaleTo::create(0.1, 0);
        //                auto scale2 = ScaleTo::create(0.7f, ICON_SCALE * 1.05f);
        
        auto seq = Sequence::create(fadeIn, fadeOut, nullptr);
        sender->runAction(RepeatForever::create(seq));
    });
    
    btn->runAction(Sequence::create(fadeOut, callFuncN, nullptr));
}
