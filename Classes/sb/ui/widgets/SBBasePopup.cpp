//
//  SBBasePopup.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "SBBasePopup.hpp"

#include "../../base/SBMacros.h"
#include "../../utils/SBNodeUtils.hpp"

USING_NS_CC;
using namespace std;

SBBasePopup::SBBasePopup() {
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
}

SBBasePopup::~SBBasePopup() {
    
}

bool SBBasePopup::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    // 터치 방지
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [=](Touch *touch, Event*) -> bool {
        
        if( !SBNodeUtils::hasVisibleParents(this) ) {
            return false;
        }
        
        this->onTouch(this, touch, SBTouchEventType::BEGAN);
        
        return true;
    };
    listener->onTouchEnded = [=](Touch *touch, Event*) {
        
        this->onTouch(this, touch, SBTouchEventType::ENDED);
    };
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}
