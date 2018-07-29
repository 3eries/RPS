//
//  AdsEventDispatcher.cpp
//
//  Created by seongmin hwang on 2018. 4. 9..
//

#include "AdsEventDispatcher.hpp"

#include "../../utils/SBCollection.hpp"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

AdsEventDispatcher::AdsEventDispatcher() {
}

AdsEventDispatcher::~AdsEventDispatcher() {
    
    listeners.clear();
}

/**
 * 이벤트 리스너 추가
 */
void AdsEventDispatcher::addListener(AdListener *listener) {
    
    if( listener && !listeners.contains(listener) ) {
        this->listeners.pushBack(listener);
    }
}

void AdsEventDispatcher::addListener(AdType type, AdListener *listener) {
 
    if( listener ) {
        listener->setType(type);
        addListener(listener);
    }
}

/**
 * 이벤트 리스너 삭제
 */
void AdsEventDispatcher::removeListener(AdListener *listener) {
    
    if( listener && listeners.contains(listener) ) {
        this->listeners.eraseObject(listener);
    }
}

void AdsEventDispatcher::removeListener(Node *target) {
    
    if( !target ) {
        return;
    }
    
    SBCollection::remove(this->listeners, [=](AdListener *listener) {
        return listener->getTarget() == target;
    });
}

void AdsEventDispatcher::removeListeners(AdListeners listeners, bool remainForever) {
    
    for( auto listener : listeners ) {
        if( remainForever && listener->isForever() ) {
            continue;
        }
        
        removeListener(listener);
    }
}

/**
 * onAdLoaded 이벤트 전달
 */
void AdsEventDispatcher::dispatchLoadedEvent(AdType type) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = findListeners([=](AdListener *listener) -> bool {
            return listener->getType() == type && listener->onAdLoaded != nullptr;
        });
        
        for( auto listener : listeners ) {
            listener->onAdLoaded();
        }
        
        listeners.clear();
    });
}

/**
 * onAdFailedToLoad 이벤트 전달
 */
void AdsEventDispatcher::dispatchFailedToLoadEvent(AdType type, int errorCode) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = findListeners([=](AdListener *listener) -> bool {
            return listener->getType() == type;
        });
        
        for( auto listener : listeners ) {
            if( listener->onAdFailedToLoad ) {
                listener->onAdFailedToLoad(errorCode);
            }
        }
        
        removeListeners(listeners);
        listeners.clear();
    });
}

/**
 * onAdOpened 이벤트 전달
 */
void AdsEventDispatcher::dispatchOpenedEvent(AdType type) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = findListeners([=](AdListener *listener) -> bool {
            return listener->getType() == type && listener->onAdOpened != nullptr;
        });
        
        for( auto listener : listeners ) {
            listener->setOpened(true);
            listener->onAdOpened();
        }
        
        listeners.clear();
    });
}

/**
 * onAdClosed 이벤트 전달
 */
void AdsEventDispatcher::dispatchClosedEvent(AdType type) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = findListeners([=](AdListener *listener) -> bool {
            return listener->getType() == type;
        });
        
        for( auto listener : listeners ) {
            listener->setOpened(false);
            
            if( listener->onAdClosed ) {
                listener->onAdClosed();
            }
        }
        
        removeListeners(listeners);
        listeners.clear();
    });
}

/**
 * onAdLoaded 이벤트 전달
 */
void AdsEventDispatcher::dispatchRewardedEvent(string type, int amount) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = findListeners([=](AdListener *listener) -> bool {
            return dynamic_cast<RewardedVideoAdListener*>(listener) != nullptr;
        });
        
        for( auto listener : listeners ) {
            auto rewardedVideoAdListener = dynamic_cast<RewardedVideoAdListener*>(listener);
            rewardedVideoAdListener->setRewarded(true);
            
            if( rewardedVideoAdListener->onRewarded ) {
                rewardedVideoAdListener->onRewarded(type, amount);
            }
        }
        
        listeners.clear();
    });
}

NS_SB_END;
