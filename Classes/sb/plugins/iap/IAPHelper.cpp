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
NS_IAP_BEGIN;

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

IAPHelper::IAPHelper() :
state(State::IDLE) {
}

IAPHelper::~IAPHelper() {
    listeners.clear();
}

void IAPHelper::init(const Config &config, const string &originalJson) {
    
    this->config = config;
    
    // TODO: config 검증 (json)
    
    initImpl(config, originalJson);
}

void IAPHelper::setDebug(bool debug) {
    
}

/**
 * 아이템 반환
 */
vector<Item> IAPHelper::getItems() {
    
    return getInstance()->config.items;
}

Item IAPHelper::getItemByName(const string &name) {
    
    auto items = getItems();
    
    for( auto item : items ) {
        if( item.name == name ) {
            return item;
        }
    }
    
    return Item();
}

Item IAPHelper::getItemById(const string &itemId) {
    
    auto items = getItems();
    
    for( auto item : items ) {
        if( item.itemId == itemId ) {
            return item;
        }
    }
    
    return Item();
}

/**
 * 광고 제거 아이템 반환
 */
Item IAPHelper::getRemoveAdsItem() {
    
    auto items = getItems();
    
    for( auto item : items ) {
        if( item.type == ItemType::REMOVE_ADS ) {
            return item;
        }
    }
    
    return Item();
}

/**
 * 아이템 구매
 */
bool IAPHelper::purchase(const string &name, PurchaseListener *listener) {
    
    auto item = getItemByName(name);
    
    if( item.itemId == "" ) {
        CCLOG("IAPHelper::purchase warning: not found item.");
        return false;
    }
    
    getInstance()->state = State::PURCHASE;
    
    if( listener ) {
        getInstance()->addListener(listener);
    }
    
    purchaseImpl(item);
    
    return true;
}

/**
 * 광고 아이템 구매
 */
bool IAPHelper::purchaseRemoveAds(PurchaseListener *listener) {
    
    return purchase(getRemoveAdsItem().name, listener);
}

/**
 * 아이템 복원
 */
void IAPHelper::restore(RestoreListener *listener) {
    
    getInstance()->state = State::RESTORE;
    
    if( listener ) {
        getInstance()->addListener(listener);
    }
    
    restoreImpl();
}

/**
 * 구매 성공
 */
void IAPHelper::onPurchased(const string &itemId) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        // auto listeners = this->getPurchaseListeners();
        auto listeners = this->listeners;
        
        const auto item = this->getItemById(itemId);
        const bool isRemoveAdsItem = (item.type == ItemType::REMOVE_ADS);
        
        for( auto listener : listeners ) {
            if( isRemoveAdsItem && listener->onRemoveAds ) {
                listener->onRemoveAds();
            }
            
            if( listener->onPurchased ) {
                listener->onPurchased(item);
            }
        }
        
        // this->removeListeners(SBCollection::convert<PurchaseListener*, Listener*>(listeners));
        this->removeListeners(listeners);
    });
}

/**
 * 구매 실패
 */
void IAPHelper::onPurchaseError(const string &errorMsg) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = this->getPurchaseListeners();
        
        for( auto listener : listeners ) {
            if( listener->onError ) {
                listener->onError(errorMsg);
            }
        }
        
        this->removeListeners(SBCollection::convert<PurchaseListener*, Listener*>(listeners));
    });
}

/**
 * 구매 취소
 */
void IAPHelper::onPurchaseCanceled() {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = this->getPurchaseListeners();
        
        for( auto listener : listeners ) {
            if( listener->onCanceled ) {
                listener->onCanceled();
            }
        }
        
        this->removeListeners(SBCollection::convert<PurchaseListener*, Listener*>(listeners));
    });
}


/**
 * 아이템 복원 성공
 */
void IAPHelper::onRestored(const string &itemId) {
    
    onPurchased(itemId);
    
    /*
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = this->getRestoreListeners();
        
        const auto item = this->getItemById(itemId);
        const bool isRemoveAdsItem = (item.type == ItemType::REMOVE_ADS);
        
        for( auto listener : listeners ) {
            if( isRemoveAdsItem && listener->onRemoveAds ) {
                listener->onRemoveAds();
            }
            
            if( listener->onPurchased ) {
                listener->onPurchased(item);
            }
        }
        
        this->removeListeners(SBCollection::convert<RestoreListener*, Listener*>(listeners));
    });
     */
}

/**
 * 아이템 복원 실패
 */
void IAPHelper::onRestoreError(const string &errorMsg) {
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        
        auto listeners = this->getRestoreListeners();
        
        for( auto listener : listeners ) {
            if( listener->onError ) {
                listener->onError(errorMsg);
            }
        }
        
        this->removeListeners(SBCollection::convert<RestoreListener*, Listener*>(listeners));
    });
}

/**
 * 리스너 등록
 */
void IAPHelper::addListener(Listener *listener) {
    
    CCASSERT(listener != nullptr, "IAPHelper::addListener error: listener must be not null");
    
    if( !listeners.contains(listener) ) {
        listeners.pushBack(listener);
    }
}

void IAPHelper::addListener(Ref *target, Listener *listener) {
    
    CCASSERT(target != nullptr, "IAPHelper::addListener error: target must be not null");
    CCASSERT(listener != nullptr, "IAPHelper::addListener error: listener must be not null");
    
    listener->setTarget(target);
    addListener(listener);
}

/**
 * 리스너 제거
 */
void IAPHelper::removeListener(Listener *listener) {
    
    CCASSERT(listener != nullptr, "IAPHelper::removeListener error: listener must be not null");
    
    if( listeners.contains(listener) ) {
        listeners.eraseObject(listener);
    }
}

void IAPHelper::removeListener(Ref *target) {
    
    CCASSERT(target != nullptr, "IAPHelper::removeListener error: target must be not null");
    
    auto removeListeners = SBCollection::find(listeners, [=](Listener *listener) -> bool {
        return listener->getTarget() == target;
    });
    
    for( auto listener : removeListeners ) {
        listeners.eraseObject(listener);
    }
}

void IAPHelper::removeListeners(vector<Listener*> listeners, bool remainForever) {
    
    for( auto listener : listeners ) {
        if( remainForever && listener->isForever() ) {
            continue;
        }
        
        removeListener(listener);
    }
}

void IAPHelper::removeListeners(Vector<Listener*> listeners, bool remainForever) {
    
    vector<Listener*> vector;
    
    for( auto listener : listeners ) {
        vector.push_back(listener);
    }
    
    removeListeners(vector, remainForever);
}

vector<PurchaseListener*> IAPHelper::getPurchaseListeners() {

    vector<PurchaseListener*> purchaseListeners;
    
    for( auto listener : listeners ) {
        auto purchaseListener = dynamic_cast<PurchaseListener*>(listener);
        
        if( purchaseListener ) {
            purchaseListeners.push_back(purchaseListener);
        }
    }
    
    return purchaseListeners;
}

vector<RestoreListener*> IAPHelper::getRestoreListeners() {
    
    vector<RestoreListener*> restoreListeners;
    
    for( auto listener : listeners ) {
        auto restoreListener = dynamic_cast<RestoreListener*>(listener);
        
        if( restoreListener ) {
            restoreListeners.push_back(restoreListener);
        }
    }
    
    return restoreListeners;
}

NS_IAP_END;
NS_SB_END;
