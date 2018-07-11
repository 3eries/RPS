//
//  IAPListener.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#include "IAPListener.hpp"

NS_SB_BEGIN;

IAPListener::IAPListener() :
target(nullptr),
onPurchased(nullptr), onFailure(nullptr), onCanceled(nullptr),
onRemoveAdsPurchased(nullptr),
onRestored(nullptr), onRestoreFinished(nullptr) {
    
}

IAPListener::~IAPListener() {
    
}

NS_SB_END;
