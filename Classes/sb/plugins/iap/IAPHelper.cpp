//
//  IAPHelper.cpp
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#include "IAPHelper.hpp"

#include "../../utils/SBCollection.hpp"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

static IAPHelper *instance = nullptr;
IAPHelper* IAPHelper::getInstance() {
    
    if( !instance ) {
        instance = new IAPHelper();
    }
    
    return instance;
}

void IAPHelper::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

IAPHelper::IAPHelper() {
}

IAPHelper::~IAPHelper() {
    listeners.clear();
}

void IAPHelper::init() {
    
}

/**
 * 리스너 등록
 */
void IAPHelper::addListener(IAPListener *listener) {
    
    CCASSERT(listener != nullptr, "IAPHelper::addListener error: listener must be not null");
    
    if( !listeners.contains(listener) ) {
        listeners.pushBack(listener);
    }
}

void IAPHelper::addListener(Node *target, IAPListener *listener) {
    
    CCASSERT(target != nullptr, "IAPHelper::addListener error: target must be not null");
    CCASSERT(listener != nullptr, "IAPHelper::addListener error: listener must be not null");
    
    listener->setTarget(target);
    addListener(listener);
}

/**
 * 리스너 제거
 */
void IAPHelper::removeListener(IAPListener *listener) {
    
    CCASSERT(listener != nullptr, "IAPHelper::removeListener error: listener must be not null");
    
    if( listeners.contains(listener) ) {
        listeners.eraseObject(listener);
    }
}

void IAPHelper::removeListener(Node *target) {
    
    CCASSERT(target != nullptr, "IAPHelper::removeListener error: target must be not null");
    
    auto removeListeners = SBCollection::find(listeners, [=](IAPListener *listener) -> bool {
        return listener->getTarget() == target;
    });
    
    for( auto listener : removeListeners ) {
        listeners.eraseObject(listener);
    }
}

void IAPHelper::onRemoveAdsPurchased() {
    
    auto prod = Product();
    
    for( auto listener : listeners ) {
        if( listener->onRemoveAdsPurchased ) {
            listener->onRemoveAdsPurchased(prod);
        }
    }
}

NS_SB_END;
