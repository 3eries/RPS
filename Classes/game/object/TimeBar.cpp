//
//  TimeBar.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#include "TimeBar.hpp"

#include "Define.h"
#include "GameConfiguration.hpp"
#include "UserDefaultKey.h"
#include "TestHelper.hpp"

#include "../GameDefine.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

TimeBar::TimeBar() :
onTimeChangedListener(nullptr),
gameMgr(GameManager::getInstance()),
started(false) {
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
    
    reset();
    
    gameMgr->addListener(this);
    
    return true;
}

void TimeBar::onEnter() {
    
    Sprite::onEnter();
    
    if( gameMgr->hasBoostItem() ) {
        setTimePoint(gameMgr->getConfig()->getTimeInfo().maxPoint);
    }
}

void TimeBar::onExit() {
    
    gameMgr->removeListener(this);
    
    Sprite::onExit();
}

void TimeBar::reset() {
    
    setVisible(true);
    
#if ENABLE_TEST_MENU
    setVisible(TestHelper::getInstance()->isTimeBarEnabled());
#endif
    
    setTimePoint(gameMgr->getConfig()->getTimeInfo().firstPoint);
}

void TimeBar::onGameStart() {
 
    reset();
    
    if( gameMgr->hasBoostItem() ) {
        setTimePoint(gameMgr->getConfig()->getTimeInfo().maxPoint);
    }
}

void TimeBar::onGameRestart() {
}

void TimeBar::onGamePause() {
}

void TimeBar::onGameResume() {
}

/**
 * 게임 오버 전
 */
void TimeBar::onPreGameOver() {
    
    setVisible(false);
    stopTimeBar();
}

/**
 * 이어하기
 */
void TimeBar::onContinue() {
    
    reset();
    
    setTimePoint(gameMgr->getConfig()->getTimeInfo().continuePoint);
}

void TimeBar::onGameOver() {
    
}

/**
 * 부스트 시작
 */
void TimeBar::onBoostStart() {
    
}

/**
 * 타이머 시작
 */
void TimeBar::onStartTimer() {
    
    startTimeBar();
}

/**
 * 시작
 */
void TimeBar::startTimeBar() {
    
    if( started ) {
        return;
    }
    
    started = true;
    scheduleUpdate();
}

/**
 * 중지
 */
void TimeBar::stopTimeBar() {
    
    started = false;
    unscheduleUpdate();
}

/**
 * 타임 포인트 설정
 */
void TimeBar::setTimePoint(float point, bool isUpdateGage) {
    
    point = MAX(0, point);
    point = MIN(gameMgr->getConfig()->getTimeInfo().maxPoint, point);
    
    this->timePoint = point;
    
//    CCLOG("timePoint: %f", point);
    
    if( isUpdateGage ) {
        updateGage();
    }
}

/**
 * 타임 포인트 증가
 */
void TimeBar::increaseTimePoint(float point) {
    
    CCASSERT(point > 0, "TimeBar::increaseTimePoint error: invalid point.");
    
    setTimePoint(timePoint + point);
}

float TimeBar::getTimeRatio() {
    
    return timePoint / gameMgr->getConfig()->getTimeInfo().maxPoint;
}

void TimeBar::update(float dt) {
    
    // update time point
    float decreasePoint = gameMgr->getLevelInfo().decreasePointPerSeconds * dt;
    float newTimePoint = timePoint - decreasePoint;
    
    setTimePoint(newTimePoint);
    
    // 타임 포인트 체크
    if( timePoint == 0.0f ) {
        GameManager::getInstance()->onPreGameOver();
    }
}

/**
 * 게이지 업데이트
 */
void TimeBar::updateGage() {
    
    float ratio = getTimeRatio();
    float per = ratio * 100;
    gage->setPercentage(per);
    
    if( onTimeChangedListener ) {
        onTimeChangedListener(timePoint, ratio);
    }
}
