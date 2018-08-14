//
//  User.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "User.hpp"

#include "SceneManager.h"
#include "GameConfiguration.hpp"
#include "UserDefaultKey.h"

#include "ReviewPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static User *instance = nullptr;
User* User::getInstance() {
    
    if( !instance ) {
        instance = new User();
    }
    
    return instance;
}

void User::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

User::User() {
    
}

User::~User() {
    
}

void User::init() {
    
    if( isOwnRemoveAdsItem() ) {
        removeAds();
    }
}

/**
 * 광고 제거 아이템 보유 여부
 */
bool User::isOwnRemoveAdsItem() {
    return UserDefault::getInstance()->getBoolForKey(UserDefaultKey::OWN_REMOVE_ADS_ITEM, false);
}

void User::setOwnRemoveAdsItem(bool owned) {
    
    UserDefault::getInstance()->setBoolForKey(UserDefaultKey::OWN_REMOVE_ADS_ITEM, owned);
    UserDefault::getInstance()->flush();
}

/**
 * 광고 제거
 */
void User::removeAds() {

    setOwnRemoveAdsItem(true);

//    AdsManager::getInstance()->setActiveBanner(false);
//    AdsManager::getInstance()->setActiveInterstitial(false);
/**
 * 리뷰 체크
 */
bool User::checkReview(float popupDelay) {
    
    if( !ReviewHelper::isReviewAlertEnabled() ) {
        return false;
    }
    
    auto showPopup = [=]() {
        ReviewHelper::showReviewPopup([=]() {
            
            // 커스텀 팝업
            auto popup = ReviewPopup::create();
            popup->setOnGoListener([=]() {
                
                ReviewHelper::setReviewWrite(true);
                Application::getInstance()->openURL(GameConfiguration::getInstance()->getStoreUrl());
                
                popup->dismissWithAction();
            });
            SceneManager::getScene()->addChild(popup, PopupZOrder::MIDDLE);
        });
    };
    
    if( popupDelay == 0 ) {
        showPopup();
    } else {
        SBDirector::postDelayed(SceneManager::getScene(), showPopup, popupDelay, true);
    }
    
    return true;
}
