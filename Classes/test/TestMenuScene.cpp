//
//  TestMenuScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "TestMenuScene.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

#include "TouchTestScene.hpp"
#include "ConfigTestScene.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

TestMenuScene::TestMenuScene() {
    
}

TestMenuScene::~TestMenuScene() {
    
}

bool TestMenuScene::init() {
    
    if( !BaseTestScene::init() ) {
        return false;
    }
    
    auto touchTestBtn = UIHelper::createFontButton("TOUCH", ButtonSize::MEDIUM);
    touchTestBtn->setAnchorPoint(ANCHOR_M);
    touchTestBtn->setPosition(Vec2MC(0, 0));
    addChild(touchTestBtn);
    
    touchTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(TouchTestScene::create());
    });
    
    auto configTestBtn = UIHelper::createFontButton("CONFIG", ButtonSize::MEDIUM);
    configTestBtn->setAnchorPoint(ANCHOR_M);
    configTestBtn->setPosition(Vec2MC(0, -100));
    addChild(configTestBtn);
    
    configTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(ConfigTestScene::create());
    });
    
    return true;
}
