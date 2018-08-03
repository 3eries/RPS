//
//  AdsHelper-android.cpp
//
//  Created by seongmin hwang on 2018. 4. 10..
//

#include "AdsHelper.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

#define JNI_CLASS_NAME     "com/superbomb/plugins/ads/AdsManager"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

/**
 * 광고 모듈 초기화
 */
void AdsHelper::init(const AdsConfig &config) {
    
    if( isInitialized ) {
        return;
    }
    
    isInitialized = true;
    this->config = config;
    
    initListeners();
    
    // 매니저 초기화
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "initAd",
                                             config.appId,
                                             config.bannerUnitId,
                                             config.interstitialUnitId,
                                             config.rewardedVideoUnitId);
}


/**
 * 배너 광고 로드
 */
void AdsHelper::loadBanner() {
    
    if( !banner.active ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "loadBanner");
}

/**
 * 전면 광고 로드
 */
void AdsHelper::loadInterstitial() {
    
    if( !interstitial.active ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "loadInterstitial");
}

/**
 * 보상형 동영상 광고 로드
 */
void AdsHelper::loadRewardedVideo() {
    
    if( !rewardedVideo.active ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "loadRewardedVideo");
}

/**
 * 배너 광고
 */
void AdsHelper::showBanner(AdListener *listener) {
    
    if( !getInstance()->getBanner().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::BANNER, listener);
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "showBanner");
}

void AdsHelper::hideBanner() {

    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "hideBanner");
}

/**
 * 전면 광고
 */
void AdsHelper::showInterstitial(AdListener *listener) {
    
    if( !getInstance()->getInterstitial().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::INTERSTITIAL, listener);
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "showInterstitial");
}

/**
 * 보상형 비디오
 */
void AdsHelper::showRewardedVideo(AdListener *listener) {
    
    if( !getInstance()->getRewardedVideo().active ) {
        return;
    }
    
    getInstance()->getEventDispatcher()->addListener(AdType::REWARDED_VIDEO, listener);
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "showRewardedVideo");
}

bool AdsHelper::isBannerVisible() {
    return cocos2d::JniHelper::callStaticBooleanMethod(JNI_CLASS_NAME, "isBannerVisible");
}

bool AdsHelper::isBannerLoaded() {
    return cocos2d::JniHelper::callStaticBooleanMethod(JNI_CLASS_NAME, "isBannerLoaded");
}

bool AdsHelper::isInterstitialLoaded() {
    return cocos2d::JniHelper::callStaticBooleanMethod(JNI_CLASS_NAME, "isInterstitialLoaded");
}

bool AdsHelper::isRewardedVideoLoaded() {
    return cocos2d::JniHelper::callStaticBooleanMethod(JNI_CLASS_NAME, "isRewardedVideoLoaded");
}

extern "C" {

    /**
     * Banner
     */
    void Java_com_superbomb_plugins_ads_AdsManager_nativeBannerOnAdLoaded(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getBanner().listener->onAdLoaded();
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeBannerOnAdFailedToLoad(JNIEnv *env, jobject obj, jint error) {
        AdsHelper::getInstance()->getBanner().listener->onAdFailedToLoad(error);
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeBannerOnAdOpened(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getBanner().listener->onAdOpened();
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeBannerOnAdClosed(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getBanner().listener->onAdClosed();
    }
    
    /**
     * Interstitial
     */
    void Java_com_superbomb_plugins_ads_AdsManager_nativeInterstitialOnAdLoaded(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getInterstitial().listener->onAdLoaded();
    }

    void Java_com_superbomb_plugins_ads_AdsManager_nativeInterstitialOnAdFailedToLoad(JNIEnv *env, jobject obj, jint error) {
        AdsHelper::getInstance()->getInterstitial().listener->onAdFailedToLoad(error);
    }

    void Java_com_superbomb_plugins_ads_AdsManager_nativeInterstitialOnAdOpened(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getInterstitial().listener->onAdOpened();
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeInterstitialOnAdClosed(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getInterstitial().listener->onAdClosed();
    }
    
    /**
     * Rewarded Video
     */
    void Java_com_superbomb_plugins_ads_AdsManager_nativeRewardedVideoOnAdLoaded(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getRewardedVideo().listener->onAdLoaded();
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeRewardedVideoOnAdFailedToLoad(JNIEnv *env, jobject obj, jint error) {
        AdsHelper::getInstance()->getRewardedVideo().listener->onAdFailedToLoad(error);
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeRewardedVideoOnAdOpened(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getRewardedVideo().listener->onAdOpened();
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeRewardedVideoOnAdClosed(JNIEnv *env, jobject obj) {
        AdsHelper::getInstance()->getRewardedVideo().listener->onAdClosed();
    }
    
    void Java_com_superbomb_plugins_ads_AdsManager_nativeRewardedVideoOnRewarded(JNIEnv *env, jobject obj, jstring jtype, jint rewardAmount) {
        string type = JniHelper::jstring2string(jtype);
        
        auto listener = dynamic_cast<RewardedVideoAdListener*>(AdsHelper::getInstance()->getRewardedVideo().listener);
        listener->onRewarded(type, rewardAmount);
    }
}

NS_SB_END;

#endif // CC_PLATFORM_ANDROID