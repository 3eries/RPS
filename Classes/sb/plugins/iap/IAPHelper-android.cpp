//
//  IAPHelper-android.cpp
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "IAPHelper.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

#define JNI_CLASS_NAME     "com/superbomb/plugins/iap/IAPHelper"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;
NS_IAP_BEGIN;

void IAPHelper::initImpl(const Config &config, const string &originalJson) {
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "initIAP", originalJson);
}

void IAPHelper::purchaseImpl(const Item &item) {
    
    bool consumable = (item.type == ItemType::CONSUMABLE);
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "purchase", item.itemId, consumable);
}

void IAPHelper::restoreImpl() {
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "restore");
}

extern "C" {
    
    void Java_com_superbomb_plugins_iap_IAPManager_nativeOnPurchased(JNIEnv *env, jobject obj) {
    }
}

NS_IAP_END;
NS_SB_END;

#endif // CC_PLATFORM_ANDROID
