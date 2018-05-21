//
//  RSPButton.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 15..
//

#include "RSPButton.hpp"

#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

RSPButton* RSPButton::create(RSPType type) {
    
    auto btn = new RSPButton(type);
    
    if( btn && btn->init() ) {
        btn->autorelease();
        return btn;
    }
    
    CC_SAFE_DELETE(btn);
    return nullptr;
}

RSPButton::RSPButton(RSPType type) :
type(type),
onClickListener(nullptr) {
    
}

RSPButton::~RSPButton() {
    
}

bool RSPButton::init() {
    
    if( !Widget::init() ) {
        return false;
    }
    
    initImage();
    initTouch();
    
    return true;
}

void RSPButton::initImage() {
    
    img = Sprite::create(UIHelper::getImageFile(type));
    img->setAnchorPoint(ANCHOR_M);
    img->setPosition(Vec2MC(img->getContentSize(), 0, 0));
//    img->setColor(UIHelper::getColor(type));
    addChild(img, -1);
    
    setContentSize(img->getContentSize());
}

void RSPButton::initTouch() {
    
    setTouchEnabled(true);
    
    addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        switch( eventType ) {
            case Widget::TouchEventType::BEGAN: {
                SB_SAFE_PERFORM_LISTENER_N(this, onClickListener);
                
                // action
                img->stopAllActions();
                
                auto scale1 = ScaleTo::create(0.07f, 1.07f);
                auto scale2 = ScaleTo::create(0.05f, 1.0f);
                img->runAction(Sequence::create(scale1, scale2, nullptr));
                
            } break;
                
            case Widget::TouchEventType::ENDED:
            case Widget::TouchEventType::CANCELED: {
                
            } break;
                
            default: break;
        }
    });
}

