//
//  Man.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#include "Man.hpp"

#include "../GameDefine.h"

USING_NS_CC;
using namespace std;

static const vector<string> FILES({
    DIR_IMG_GAME + "RSP_avatar_idle.png",
    DIR_IMG_GAME + "RSP_avatar_attack.png",
});

static const float DELAY_PER_UNIT = 0.05f;

Man::Man() :
lastHitType(RSPType::NONE) {
}

Man::~Man() {
}

bool Man::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    initImage();
    initFeverGage();
    
    setAnchorPoint(ANCHOR_MB);
    setPosition(MAN_POS_LEFT);
    setManPosition(Position::LEFT);
    
//    int ran = arc4random() % 2;
//    setPosition((ran == 0) ? MAN_POS_LEFT : MAN_POS_RIGHT);
    
    return true;
}

void Man::setManPosition(Position pos) {
    
    manPosition = pos;
    
    switch( pos ) {
        case Position::LEFT: {
            img->setFlippedX(false);
            setPosition(MAN_POS_LEFT);
            
            feverGage.bg->setPosition(Vec2TC(getContentSize(), -24, 12));
            
        } break;
            
        case Position::RIGHT: {
            img->setFlippedX(true);
            setPosition(MAN_POS_RIGHT);
            
            feverGage.bg->setPosition(Vec2TC(getContentSize(), 24, 12));
            
        } break;
            
        default:
            CCASSERT(false, "Man::setManPosition error: invalid position.");
            break;
    }
}

void Man::showdown(/*RSPType type*/) {
    
    // 무승부
    auto blink = Blink::create(DRAW_DELAY, 2);
    img->runAction(blink);
}

void Man::hit(RSPType type) {
    
    lastHitType = type;
    
    SBAudioEngine::play2d(SOUND_PUNCH);
    
    img->stopAnimation();
    img->runAnimation();
    
    switch( type ) {
        case RSPType::ROCK:     setManPosition(Position::LEFT);     break;
        case RSPType::PAPER:    setManPosition(Position::RIGHT);    break;
        case RSPType::SCISSORS: {
            
        } break;
            
        default:
            CCASSERT(false, "Man::hit error: invalid block type.");
            break;
    }
}

/**
 * 이미지 초기화
 */
void Man::initImage() {
    
    auto anim = SBNodeUtils::createAnimation(FILES, DELAY_PER_UNIT);
    anim->setRestoreOriginalFrame(true);
    
    img = SBAnimationSprite::create(anim, 1);
    img->setAnchorPoint(ANCHOR_M);
    img->setPosition(Vec2MC(img->getContentSize(), 0, 0));
    addChild(img);
    
    setContentSize(img->getContentSize());
}

/**
 * 피버 게이지 초기화
 */
void Man::initFeverGage() {
 
    feverGage.bg = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_bg.png");
    feverGage.bg->setAnchorPoint(ANCHOR_MB);
    feverGage.bg->setPosition(Vec2TC(getContentSize(), -24, 12));
    addChild(feverGage.bg);
    
    auto bar = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_green.png");
    
    feverGage.gage = ProgressTimer::create(bar);
    feverGage.gage->setType(ProgressTimer::Type::BAR);
    feverGage.gage->setMidpoint(ANCHOR_ML);
//    feverGage.gage->setBarChangeRate(Vec2(1, 0));
    feverGage.gage->setAnchorPoint(ANCHOR_M);
    feverGage.gage->setPosition(Vec2MC(feverGage.bg->getContentSize(), 0, 0));
    feverGage.gage->setScaleY(1.44f);
    feverGage.bg->addChild(feverGage.gage);
    
    feverGage.gage->setPercentage(50);
}
