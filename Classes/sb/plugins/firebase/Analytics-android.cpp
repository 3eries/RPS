//
//  Analytics.cpp
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "Analytics.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "../../platform/android/SBJniHelper.hpp"
#include "../../json/SBJSON.h"

#include "platform/android/jni/JniHelper.h"

#define JNI_CLASS_NAME     "com/superbomb/plugins/firebase/Analytics"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

namespace firebase {
    
void Analytics::setScreenName(const string &screen, const string &screenClass) {

    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "setScreenName", screen, screenClass);
}

void Analytics::logEvent(const string &event, const EventParams &params) {

    string jsonParam = "";
    
    if( params.size() > 0 ) {
        jsonParam = SBJSON::toJSON(Value(params));
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "logEvent", event, jsonParam);
}
    
} // namespace firebase

NS_SB_END;

#endif // CC_PLATFORM_ANDROID
