//
//  AdsHelper-ios.mm
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#include "AdsHelper.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import "ios/AdsManager.h"

#include "../../platform/SBPlatformMacros.h"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

/**
 * 광고 모듈 초기화
 */
void AdsHelper::initImpl(const AdsConfig &config) {
    
    // 매니저 초기화
    AdsManager *mgr = [AdsManager getInstance];
    [mgr initAd:config];
    
    mgr.onBannerListener = banner.listener;
    mgr.onInterstitialListener = interstitial.listener;
    mgr.onRewardedVideoListener = (RewardedVideoAdListener*)rewardedVideo.listener;
}

/**
 * 배너 광고 로드
 */
void AdsHelper::loadBanner() {
    
    if( !banner.active ) {
        return;
    }
    
    [[AdsManager getInstance] loadBanner];
}

/**
 * 전면 광고 로드
 */
void AdsHelper::loadInterstitial() {
    
    if( !interstitial.active ) {
        return;
    }
    
    [[AdsManager getInstance] loadInterstitial];
}

/**
 * 보상형 동영상 광고 로드
 */
void AdsHelper::loadRewardedVideo() {
    
    if( !rewardedVideo.active ) {
        return;
    }
    
    [[AdsManager getInstance] loadRewardedVideo];
}

/**
 * 배너 광고
 */
void AdsHelper::showBanner(AdListener *listener) {
    
    if( !getInstance()->getBanner().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::BANNER, listener);
    
    [[AdsManager getInstance] showBanner];
}

void AdsHelper::hideBanner() {

    /*
    if( !banner.active ) {
        return;
    }
     */
    
    [[AdsManager getInstance] hideBanner];
}

/**
 * 전면 광고
 */
void AdsHelper::showInterstitial(AdListener *listener) {
    
    if( !getInstance()->getInterstitial().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::INTERSTITIAL, listener);
    
    [[AdsManager getInstance] showInterstitial];
}

/**
 * 보상형 비디오
 */
void AdsHelper::showRewardedVideo(AdListener *listener) {
    
    if( !getInstance()->getRewardedVideo().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::REWARDED_VIDEO, listener);
    
    [[AdsManager getInstance] showRewardedVideo];
}

bool AdsHelper::isBannerVisible() {
    return [[AdsManager getInstance] isBannerVisible];
}

bool AdsHelper::isBannerLoaded() {
    return [[AdsManager getInstance] isBannerLoaded];
}

bool AdsHelper::isInterstitialLoaded() {
    return [[AdsManager getInstance] isInterstitialLoaded];
}

bool AdsHelper::isRewardedVideoLoaded() {
    return [[AdsManager getInstance] isRewardedVideoLoaded];
}

NS_SB_END;

#endif // CC_PLATFORM_IOS
