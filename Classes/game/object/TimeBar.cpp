//
//  TimeBar.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#include "TimeBar.hpp"

#include "RSP.h"

#include "../GameDefine.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

TimeBar::TimeBar() :
duration(FIRST_PLAY_DURATION),
elapsed(0) {
}

TimeBar::~TimeBar() {
}

bool TimeBar::init() {
    
    if( !Sprite::init() ) {
        return false;
    }
    
    setTexture(DIR_IMG_GAME + "RSP_gage_timer_bg.png");
    
    gage = ProgressTimer::create(Sprite::create(DIR_IMG_GAME + "RSP_gage_timer_yellow.png"));
    gage->setType(ProgressTimer::Type::BAR);
    gage->setMidpoint(ANCHOR_ML);
    gage->setBarChangeRate(Vec2(1, 0));
    gage->setAnchorPoint(ANCHOR_M);
    gage->setPosition(Vec2MC(getContentSize(), 0, 0));
//    gage->setScaleY(1.44f);
    addChild(gage);
    
    updateGage();
    
    GameManager::getInstance()->addListener(this);
    
    return true;
}

void TimeBar::onExit() {
    
    GameManager::getInstance()->removeListener(this);
    
    Sprite::onExit();
}

void TimeBar::reset() {
    
    duration = FIRST_PLAY_DURATION;
    elapsed = 0;
}

void TimeBar::onGameStart() {
 
    scheduleUpdate();
}

void TimeBar::onGameRestart() {
    
    reset();
    updateGage();
    
    scheduleUpdate();
}

void TimeBar::onGameOver() {
    
    unscheduleUpdate();
}

void TimeBar::onGamePause() {
    
    pause();
}

void TimeBar::onGameResume() {
    
    resume();
}

/**
 * 진행 시간 증가
 */
void TimeBar::increase(float dt) {
    
    CCASSERT(dt > 0, "TimeBar::addDuration error: invalid delta time.");
    
    duration += dt;
    
    updateGage();
}

void TimeBar::update(float dt) {
    
    elapsed += dt;
    
    // 시간 초과 체크
    if( elapsed >= duration ) {
        GameManager::getInstance()->onGameOver();
    }
    
    updateGage();
}

/**
 * 게이지 업데이트
 */
void TimeBar::updateGage() {
    
    float per = (elapsed / duration) * 100;
    per = MIN(100, per);
    per = 100 - per;
    
    gage->setPercentage(per);
}
