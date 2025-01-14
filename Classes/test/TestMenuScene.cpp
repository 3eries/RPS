//
//  TestMenuScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "TestMenuScene.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "RankingManager.hpp"
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
    
    // reset ranking
    {
        auto btn = UIHelper::createFontButton("reset ranking", ButtonSize::MEDIUM);
        btn->setAnchorPoint(ANCHOR_M);
        btn->setPosition(Vec2MC(0, PADDING_Y*3));
        addChild(btn);
        
        btn->setOnClickListener([=](Node*) {
            UserDefault::getInstance()->setStringForKey(UserDefaultKey::RANKING, "");
            UserDefault::getInstance()->flush();
            
            RankingManager::getInstance()->init();
        });
    }
    
    // remove ads on/off
    {
        auto label = Label::createWithTTF("remove ads", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::RIGHT,
                                          TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(-50, PADDING_Y*2));
        addChild(label);
        
        auto checkBox = SBToggleButton::create(DIR_IMG_COMMON + "check_box_unselected.png",
                                               DIR_IMG_COMMON + "check_box_selected.png");
        checkBox->setAnchorPoint(ANCHOR_ML);
        checkBox->setPosition(Vec2MC(160, PADDING_Y*2));
        addChild(checkBox);
        
        checkBox->setSelected(User::isOwnRemoveAdsItem());
        checkBox->setOnSelectedListener([=](bool isSelected) -> bool {
            
            User::setOwnRemoveAdsItem(isSelected);
            
            if( isSelected ) {
                auto item = superbomb::iap::IAPHelper::getInstance()->getRemoveAdsItem();
                superbomb::iap::IAPHelper::getInstance()->onPurchased(item.itemId);
            }
            
            return false;
        });
    }
    
    // cheat mode on/off
    {
        auto label = Label::createWithTTF("cheat mode", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::RIGHT,
                                          TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(-50, PADDING_Y*1));
        addChild(label);
        
        auto checkBox = SBToggleButton::create(DIR_IMG_COMMON + "check_box_unselected.png",
                                               DIR_IMG_COMMON + "check_box_selected.png");
        checkBox->setAnchorPoint(ANCHOR_ML);
        checkBox->setPosition(Vec2MC(160, PADDING_Y*1));
        addChild(checkBox);
        
        checkBox->setSelected(testHelper->isCheatMode());
        checkBox->setOnSelectedListener([=](bool isSelected) -> bool {
            
            testHelper->setCheatMode(isSelected);
            
            return false;
        });
    }
    
    // time bar on/off
    {
        auto label = Label::createWithTTF("time bar", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::RIGHT,
                                          TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(-40, PADDING_Y*0));
        addChild(label);
        
        auto checkBox = SBToggleButton::create(DIR_IMG_COMMON + "check_box_unselected.png",
                                               DIR_IMG_COMMON + "check_box_selected.png");
        checkBox->setAnchorPoint(ANCHOR_ML);
        checkBox->setPosition(Vec2MC(150, PADDING_Y*0));
        addChild(checkBox);
        
        checkBox->setSelected(testHelper->isTimeBarEnabled());
        checkBox->setOnSelectedListener([=](bool isSelected) -> bool {
            
            testHelper->setTimeBarEnabled(isSelected);
            
            return false;
        });
    }
    
    // dark cloud on/off
    {
        auto label = Label::createWithTTF("dark cloud", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::RIGHT,
                                          TextVAlignment::CENTER);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(-60, -PADDING_Y*1));
        addChild(label);
        
        auto checkBox = SBToggleButton::create(DIR_IMG_COMMON + "check_box_unselected.png",
                                               DIR_IMG_COMMON + "check_box_selected.png");
        checkBox->setAnchorPoint(ANCHOR_ML);
        checkBox->setPosition(Vec2MC(150, -PADDING_Y*1));
        addChild(checkBox);
        
        checkBox->setSelected(testHelper->isDarkCloudEnabled());
        checkBox->setOnSelectedListener([=](bool isSelected) -> bool {
            
            testHelper->setDarkCloudEnabled(isSelected);
            
            return false;
        });
    }
    
    // touch
    auto touchTestBtn = UIHelper::createFontButton("touch", ButtonSize::MEDIUM);
    touchTestBtn->setAnchorPoint(ANCHOR_M);
    touchTestBtn->setPosition(Vec2MC(0, -PADDING_Y*2));
    addChild(touchTestBtn);
    
    touchTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(TouchTestScene::create());
    });
    
    // config
    auto configTestBtn = UIHelper::createFontButton("config", ButtonSize::MEDIUM);
    configTestBtn->setAnchorPoint(ANCHOR_M);
    configTestBtn->setPosition(Vec2MC(0, -PADDING_Y*3));
    addChild(configTestBtn);
    
    configTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(ConfigTestScene::create());
    });
    
    // spine
    auto spineTestBtn = UIHelper::createFontButton("spine", ButtonSize::MEDIUM);
    spineTestBtn->setAnchorPoint(ANCHOR_M);
    spineTestBtn->setPosition(Vec2MC(0, -PADDING_Y*4));
    addChild(spineTestBtn);
    
    spineTestBtn->setOnClickListener([=](Node*) {
        Director::getInstance()->pushScene(SpineTestScene::create());
    });
    
    return true;
}
