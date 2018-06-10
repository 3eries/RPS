//
//  MainView.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#include "MainView.hpp"

#include "RSP.h"
#include "UIHelper.hpp"
#include "User.hpp"

#include "SceneManager.h"

#include "../test/TestMenuScene.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

MainView::MainView() {
    
}

MainView::~MainView() {
    
}

bool MainView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    initMenu();
    
    ViewManager::getInstance()->addListener(this);
    
    return true;
}

void MainView::onEnter() {
    
    Node::onEnter();
}

void MainView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    SBAudioEngine::playBGM(SOUND_BGM_MAIN);
}

void MainView::onExit() {
    
    ViewManager::getInstance()->removeListener(this);
    
    Node::onExit();
}

void MainView::onViewChanged(ViewType viewType) {
    
}

/**
 * 게임 화면으로 이동
 */
void MainView::replaceGame() {
    
    ViewManager::getInstance()->replaceView(ViewType::GAME);
}

/**
 * 버튼 클릭
 */
void MainView::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
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

/**
 * 배경 초기화
 */
void MainView::initBg() {
    
    auto title = SBButton::create(DIR_IMG_GAME + "RSP_title.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(0, 255));
    title->setZoomScale(0);
    addChild(title);
    
    title->setOnClickListener([=](Node*) {
        // TODO: 크레딧
    });
}

/**
 * 메뉴 초기화
 */
void MainView::initMenu() {
    
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
        
        btn->setOnClickListener(CC_CALLBACK_1(MainView::onClick, this));
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


