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
    
    initBg();
    
    ViewManager::getInstance()->addListener(this);
    
    return true;
}

void MainScene::onEnter() {
    
    Scene::onEnter();
    
    ViewManager::getInstance()->onEnterMainScene(this);
}

void MainScene::onEnterTransitionDidFinish() {
    
    Scene::onEnterTransitionDidFinish();
}

void MainScene::onExit() {
    
    ViewManager::getInstance()->removeListener(this);
    
    Scene::onExit();
}

void MainScene::onViewChanged(ViewType viewType) {

    switch( viewType ) {
        case ViewType::MAIN: {
            banner->setVisible(false);
        } break;
            
        case ViewType::GAME: {
            banner->setVisible(!User::isOwnRemoveAdsItem());
        } break;
            
        default:
            break;
    }
}

void MainScene::initBg() {
    
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_bg.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg, -1);
    
    // 임시 배너 이미지
    banner = Sprite::create(DIR_IMG_GAME + "RSP_ad_top.png");
    banner->setAnchorPoint(ANCHOR_MT);
    banner->setPosition(Vec2TC(0, 0));
    addChild(banner, SBZOrder::BOTTOM);
}
