//
//  Analytics.cpp
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "Analytics.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "../../platform/android/SBJniHelper.hpp"
#include "platform/android/jni/JniHelper.h"

#define JNI_CLASS_NAME     "com/superbomb/plugins/firebase/Analytics"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

namespace firebase {
    
    void Analytics::setScreenName(const string &screen, const string &screenClass) {
        
        JniMethodInfo t;
        
        const char *methodName = "setScreenName";
        const char *paramCode = "(Ljava/lang/String;Ljava/lang/String;)V";
        
        bool find = cocos2d::JniHelper::getStaticMethodInfo(t, JNI_CLASS_NAME, methodName, paramCode);
        CCAssert(find, "Analytics::setScreenName jni error.");
        
        jstring jstr1 = t.env->NewStringUTF(screen.c_str());
        jstring jstr2 = t.env->NewStringUTF(screenClass.c_str());
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jstr1, jstr2);
        
        t.env->DeleteLocalRef(jstr1);
        t.env->DeleteLocalRef(jstr2);
        t.env->DeleteLocalRef(t.classID);
    }
    
    void Analytics::logEvent(const string &event, const EventParams &params) {

        JniMethodInfo t;
        
        const char *methodName = "logEvent";
        const char *paramCode = "(Ljava/lang/String;Ljava/util/Hashtable;)V";
        
        bool find = cocos2d::JniHelper::getStaticMethodInfo(t, JNI_CLASS_NAME, methodName, paramCode);
        CCAssert(find, "Analytics::logEvent jni error.");
        
        jstring jevent = t.env->NewStringUTF(event.c_str());
        jobject jobjMap = superbomb::JniHelper::createMapObject(t.env, params);
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jevent, jobjMap);
        
        t.env->DeleteLocalRef(jevent);
        t.env->DeleteLocalRef(jobjMap);
        t.env->DeleteLocalRef(t.classID);
    }
    
} // namespace firebase

NS_SB_END;

#endif // CC_PLATFORM_ANDROID
