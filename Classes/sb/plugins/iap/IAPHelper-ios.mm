//
//  IAPHelper-ios.m
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "IAPHelper.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import "ios/IAPManager.h"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;
NS_IAP_BEGIN;

void IAPHelper::initImpl(const Config &config, const string &originalJson) {
    
    [[IAPManager getInstance] initIAP:config];
}

void IAPHelper::purchaseImpl(const Item &item) {
    
    [[IAPManager getInstance] purchase:item.itemId];
}

void IAPHelper::restoreImpl() {
    
    [[IAPManager getInstance] restore];
}

bool IAPHelper::isReady() {
    return true;
}

NS_IAP_END;
NS_SB_END;

#endif // CC_PLATFORM_IOS
