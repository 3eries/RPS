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
#include "UserDefaultKey.h"

#include "TouchTestScene.hpp"
#include "ConfigTestScene.hpp"
#include "SpineTestScene.hpp"

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
    
    const int FONT_SIZE = 50;
    const float PADDING_Y = 120;
    
    // time bar on/off
    {
        auto label = Label::createWithTTF("TIME BAR", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::RIGHT,
                                          TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(-40, PADDING_Y));
        addChild(label);
        
        auto checkBox = SBToggleButton::create(DIR_IMG_COMMON + "check_box_unselected.png",
                                               DIR_IMG_COMMON + "check_box_selected.png");
        checkBox->setAnchorPoint(ANCHOR_ML);
        checkBox->setPosition(Vec2MC(150, PADDING_Y));
        addChild(checkBox);
        
        checkBox->setSelected(UserDefault::getInstance()->getBoolForKey(UserDefaultKey::TEST_TIME_BAR_ENABLED, true));
        checkBox->setOnSelectedListener([=](bool isSelected) -> bool {
            
            UserDefault::getInstance()->setBoolForKey(UserDefaultKey::TEST_TIME_BAR_ENABLED, isSelected);
            UserDefault::getInstance()->flush();
            
            return true;
        });
    }
    
    // touch
    auto touchTestBtn = UIHelper::createFontButton("TOUCH", ButtonSize::MEDIUM);
    touchTestBtn->setAnchorPoint(ANCHOR_M);
    touchTestBtn->setPosition(Vec2MC(0, 0));
    addChild(touchTestBtn);
    
    touchTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(TouchTestScene::create());
    });
    
    // config
    auto configTestBtn = UIHelper::createFontButton("CONFIG", ButtonSize::MEDIUM);
    configTestBtn->setAnchorPoint(ANCHOR_M);
    configTestBtn->setPosition(Vec2MC(0, -PADDING_Y));
    addChild(configTestBtn);
    
    configTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(ConfigTestScene::create());
    });
    
    // spine
    auto spineTestBtn = UIHelper::createFontButton("SPINE", ButtonSize::MEDIUM);
    spineTestBtn->setAnchorPoint(ANCHOR_M);
    spineTestBtn->setPosition(Vec2MC(0, -PADDING_Y*2));
    addChild(spineTestBtn);
    
    spineTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(SpineTestScene::create());
    });
    
    return true;
}
