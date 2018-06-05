//
//  TimeBar.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#include "TimeBar.hpp"

#include "RSP.h"
#include "GameConfiguration.hpp"

#include "../GameDefine.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

TimeBar::TimeBar() :
gameMgr(GameManager::getInstance()) {
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
    addChild(gage);
    
    GameManager::getInstance()->addListener(this);
    
    return true;
}

void TimeBar::onExit() {
    
    GameManager::getInstance()->removeListener(this);
    
    Sprite::onExit();
}

void TimeBar::reset() {
    
    timePoint = gameMgr->getConfig()->getTimeInfo().firstPoint;
}

void TimeBar::onGameStart() {
 
    reset();
    updateGage();
    
    scheduleUpdate();
}

void TimeBar::onGameRestart() {
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
 * 타임 포인트 설정
 */
void TimeBar::setTimePoint(int point) {
    
    point = MAX(0, point);
    point = MIN(gameMgr->getConfig()->getTimeInfo().maxPoint, point);
    
    this->timePoint = point;
    
//    CCLOG("timePoint: %d", point);
}

/**
 * 타임 포인트 증가
 */
void TimeBar::increaseTimePoint(int point) {
    
    CCASSERT(point > 0, "TimeBar::increaseTimePoint error: invalid point.");
    
    setTimePoint(timePoint + point);
    
    updateGage();
}

void TimeBar::update(float dt) {
    
    // update time point
    int decreasePoint = gameMgr->getLevelInfo().decreasePointPerSeconds * dt;
    int newTimePoint = timePoint - decreasePoint;
    
    setTimePoint(newTimePoint);
    updateGage();
    
    // 타임 포인트 체크
    if( timePoint == 0 ) {
        GameManager::getInstance()->onGameOver();
    }
}

/**
 * 게이지 업데이트
 */
void TimeBar::updateGage() {
    
//    float per = (elapsed / duration) * 100;
//    per = MIN(100, per);
//    per = 100 - per;
//
//    gage->setPercentage(per);
    
    const int maxPoint = gameMgr->getConfig()->getTimeInfo().maxPoint;
    
    float per = ((float)timePoint / maxPoint) * 100;
    gage->setPercentage(per);
}
