//
//  PluginPlay-android.cpp
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "PluginPlay.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

#define JNI_CLASS_NAME     "com/superbomb/plugins/gps/GameHelper"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

void PluginPlay::init() {
    
    signIn();
}

void PluginPlay::signIn(bool showLoginUI) {
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "signIn", showLoginUI);
}

void PluginPlay::signOut() {
    
    if( !isSignedIn() ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "signOut");
}

bool PluginPlay::isSignedIn() {
    
    return cocos2d::JniHelper::callStaticBooleanMethod(JNI_CLASS_NAME, "isSignedIn");
}

string PluginPlay::getPlayerId() {
    
    if( !isSignedIn() ) {
        return "";
    }
    
    return cocos2d::JniHelper::callStaticStringMethod(JNI_CLASS_NAME, "getPlayerId");
}

void PluginPlay::showAllLeaderboards() {
    
    if( !isSignedIn() ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "showAllLeaderboards");
}

void PluginPlay::showLeaderboard(const string &leaderboardName) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "showLeaderboard", leaderboardName);
}

void PluginPlay::submitScore(const string &leaderboardName, long score) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    // long to jlong issue
    // cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "submitScore", leaderboardName, score);
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "submitScore", leaderboardName, (int)score);
    
    /*
    JniMethodInfo t;
    
    const char *methodName = "submitScore";
    const char *paramCode = "(Ljava/lang/String;J)V";
    
    bool find = cocos2d::JniHelper::getStaticMethodInfo(t, JNI_CLASS_NAME, methodName, paramCode);
    CCAssert(find, "PluginPlay::submitScore jni error.");
    
    jstring jstr = t.env->NewStringUTF(leaderboardName.c_str());
    jlong jscore = (jlong)score;
    
    t.env->CallStaticVoidMethodA(t.classID, t.methodID, jstr, (jvalue*)jscore);
    
    t.env->DeleteLocalRef(jstr);
    t.env->DeleteLocalRef(t.classID);
     */
}

void PluginPlay::showAchievements() {
    
    if( !isSignedIn() ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "showAchievements");
}

void PluginPlay::loadAchievements(bool forceReload) {
    
    // TODO:
}

void PluginPlay::unlockAchievement(const string &achievementName) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "unlockAchievement", achievementName);
}

void PluginPlay::incrementAchievement(const string &achievementName, int increment) {
   
    if( !isSignedIn() ) {
        return;
    }
    
    cocos2d::JniHelper::callStaticVoidMethod(JNI_CLASS_NAME, "incrementAchievement", achievementName, increment);
}

void PluginPlay::reveal(const string &achievementName) {
 
    // TODO:
}

void PluginPlay::setSteps(const string &achievementName, double steps) {
    
    // TODO:
}

NS_SB_END;

#endif // CC_PLATFORM_ANDROID
