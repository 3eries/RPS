//
//  SBDirector.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "SBDirector.hpp"

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

