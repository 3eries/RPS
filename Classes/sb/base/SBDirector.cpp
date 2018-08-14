//
//  SBDirector.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "SBDirector.hpp"

#include "SBUserDefaultKey.h"

#include "../utils/SBSystemUtils.h"
#include "../utils/SBStringUtils.h"
#include "../audio/SBAudioEngine.hpp"

USING_NS_CC;
using namespace std;

static SBDirector *instance = nullptr;
SBDirector* SBDirector::getInstance() {
    
    if( !instance ) {
        instance = new SBDirector();
    }
    
    return instance;
}

void SBDirector::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

SBDirector::SBDirector() :
touchListener(nullptr),
screenTouchLocked(false) {
}

SBDirector::~SBDirector() {
    
    Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
}

void SBDirector::init() {
    
    // 실행 횟수 및 시간 설정
    auto userDefault = UserDefault::getInstance();
    
    const int runCount = getRunCount() + 1;
    const string nowTime = TO_STRING(mktime(SBSystemUtils::getCurrentTime()));
    
    if( runCount == 1 ) {
        // 최초 실행
        userDefault->setStringForKey(SBUserDefaultKey::FIRST_RUN_TIME, nowTime);
    }
    
    userDefault->setIntegerForKey(SBUserDefaultKey::RUN_COUNT, runCount);
    userDefault->setStringForKey(SBUserDefaultKey::LAST_RUN_TIME, nowTime);
    userDefault->flush();
    
    // log
    {
        string log;
        log += "\t" + STR_FORMAT("run count: %d", getRunCount()) + "\n";
        log += "\t" + STR_FORMAT("first run time: %s", SBSystemUtils::timeToString(getFirstRunTime(),"%Y-%m-%d %l:%M:%S").c_str()) + "\n";
        log += "\t" + STR_FORMAT("last run time: %s", SBSystemUtils::timeToString(getLastRunTime(), "%Y-%m-%d %l:%M:%S").c_str()) + "\n";
        
        CCLOG("SBDirector::init\n{\n%s}", log.c_str());
    }
    
    // 라이브러리 초기화
    SBAudioEngine::getInstance()->init();
    
    // 터치 리스너 초기화
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->retain();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [=](Touch*, Event*) -> bool {
        return instance->screenTouchLocked;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, -1);
    
    getInstance()->touchListener = touchListener;
}

void SBDirector::end() {

    SBAudioEngine::getInstance()->destroyInstance();
    
    destroyInstance();
}

/**
 * 실행 횟수를 반환 합니다
 */
int SBDirector::getRunCount() {
    
    return UserDefault::getInstance()->getIntegerForKey(SBUserDefaultKey::RUN_COUNT, 0);
}

/**
 * 첫번째 실행 여부를 반환 합니다
 */
bool SBDirector::isFirstRun() {
    
    return (getRunCount() == 1);
}

/**
 * 앱 최초 실행 시간을 반환 합니다
 */
time_t SBDirector::getFirstRunTime() {
    
    string time = UserDefault::getInstance()->getStringForKey(SBUserDefaultKey::FIRST_RUN_TIME, "");
    
    if( time != "" ) {
        return SBStringUtils::toNumber<time_t>(time);
    }
    
    return 0;
}

/**
 * 앱 마지막 실행 시간을 반환 합니다
 */
time_t SBDirector::getLastRunTime() {
    
    string time = UserDefault::getInstance()->getStringForKey(SBUserDefaultKey::LAST_RUN_TIME, "");
    
    if( time != "" ) {
        return SBStringUtils::toNumber<time_t>(time);
    }
    
    return 0;
}

/**
 * handler 노드에 액션을 적용하여 동작을 지연 시킵니다
 */
void SBDirector::postDelayed(Node *handler, SBCallback runnable,
                             float delay, bool isScreenTouchLocked) {
    
    CCASSERT(handler != nullptr, "SBDirector::postDelayed error: handler must be not null");
    CCASSERT(runnable != nullptr, "SBDirector::postDelayed error: runnable must be not null");
    
    if( isScreenTouchLocked ) {
        getInstance()->setScreenTouchLocked(true);
    }
    
    auto delayAction = DelayTime::create(delay);
    auto callFunc = CallFunc::create([=]() {
        
        if( isScreenTouchLocked ) {
            getInstance()->setScreenTouchLocked(false);
        }
        
        runnable();
    });
    handler->runAction(Sequence::create(delayAction, callFunc, nullptr));
}
