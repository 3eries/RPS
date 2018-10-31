//
//  TouchTestScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "TouchTestScene.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define SCHEDULER_TOUCH_COUNT       "SCHEDULER_TOUCH_COUNT"

TouchTestScene::TouchTestScene() :
touchCount(0) {
    
}

TouchTestScene::~TouchTestScene() {
    
}

bool TouchTestScene::init() {
    
    if( !BaseTestScene::init() ) {
        return false;
    }
    
    // title
    auto titleLabel = Label::createWithTTF("Measure the\nnumber of touches (1 sec)", FONT_RETRO, 35,
                                           Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::TOP);
    titleLabel->setAnchorPoint(ANCHOR_MT);
    titleLabel->setPosition(Vec2TC(0, -30));
    addChild(titleLabel);
    
    // touch count
    auto touchCountLabel = Label::createWithTTF("", FONT_RETRO, 40);
    touchCountLabel->setAnchorPoint(ANCHOR_M);
    touchCountLabel->setPosition(Vec2MC(0, 100));
    addChild(touchCountLabel);
    
    // touch node
    auto touchNode = SBNodeUtils::createTouchNode();
    touchNode->setVisible(false);
    addChild(touchNode);
    
    touchNode->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN: {
                ++touchCount;
                touchCountLabel->setString(TO_STRING(touchCount));
                
            } break;
                
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED: {
            } break;
                
            default: break;
        }
    });
    
    // start
    auto testStartBtn = UIHelper::createFontButton("START", ButtonSize::MEDIUM);
    testStartBtn->setAnchorPoint(ANCHOR_M);
    testStartBtn->setPosition(Vec2MC(0, 0));
    addChild(testStartBtn);
    
    testStartBtn->setOnClickListener([=](Node*) {
        testStartBtn->setVisible(false);
        touchNode->setVisible(true);
        
        touchCount = 0;
        touchCountLabel->setString("");
        
        // schedule
        this->scheduleOnce([=](float dt) {
            testStartBtn->setVisible(true);
            touchNode->setVisible(false);
            
            touchCountLabel->setString(STR_FORMAT("TOUCH COUNT : %d", touchCount));
            
        }, 1.0f, SCHEDULER_TOUCH_COUNT);
    });
    
    return true;
}
