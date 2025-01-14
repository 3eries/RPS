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

string RSPButton::getButtonImageFile(RSPType type) {
    
    string file = "";
    
    switch( type ) {
        case RSPType::ROCK:          file = "RSP_btn_rock.png";         break;
        case RSPType::SCISSORS:      file = "RSP_btn_scissors.png";     break;
        case RSPType::PAPER:         file = "RSP_btn_paper.png";        break;
        case RSPType::ROCK_N_ROLL:   file = "RSP_btn_fever.png";        break;
        default:
            CCASSERT(false, "RSPButton::getButtonImageFile error: invalid rsp type.");
            break;
    }
    
    if( SBDirector::isPadResolution() ) {
        file = DIR_IMG_GAME_IPAD + file;
    } else {
        file = DIR_IMG_GAME + file;
    }
    
    return file;
}

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
isTouchEnabled(true),
onClickListener(nullptr) {
    
}

RSPButton::~RSPButton() {
    
}

bool RSPButton::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    initImage();
    initTouch();
    
    return true;
}

void RSPButton::initImage() {
    
    img = Sprite::create(getButtonImageFile(type));
    img->setAnchorPoint(ANCHOR_M);
    img->setPosition(Vec2MC(img->getContentSize(), 0, 0));
//    img->setColor(UIHelper::getColor(type));
    addChild(img, -1);
    
    setContentSize(img->getContentSize());
}

void RSPButton::initTouch() {
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [=](Touch *touch, Event*) -> bool {
        
        if( !isTouchEnabled ) {
            return false;
        }
        
        if( !SBNodeUtils::hasVisibleParents(this) ) {
            return false;
        }
        
        if( !SBNodeUtils::isTouchInside(this, touch) ) {
            return false;
        }
        
        // 멀티 터치 수는 2개로 제한
        if( touch->getID() > 1 ) {
            return false;
        }
        
        SB_SAFE_PERFORM_LISTENER_N(this, onClickListener);
        
        // action
        img->stopAllActions();
        
        auto scale1 = ScaleTo::create(0.07f, 1.07f);
        auto scale2 = ScaleTo::create(0.05f, 1.0f);
        img->runAction(Sequence::create(scale1, scale2, nullptr));
        
        return true;
    };
    listener->onTouchEnded = [=](Touch *touch, Event*) {};
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void RSPButton::setTouchEnabled(bool isTouchEnabled) {
    this->isTouchEnabled = isTouchEnabled;
}
