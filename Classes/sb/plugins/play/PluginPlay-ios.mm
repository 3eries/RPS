//
//  PluginPlay-ios.m
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "PluginPlay.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import <Foundation/Foundation.h>
#import "ios/GameCenterHelper.h"

#include "../../platform/SBPlatformMacros.h"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

void PluginPlay::init() {
    
//    [GameCenterHelper getInstance];
    
//    if( !superbomb::PluginPlay::isSignedIn() ) {
//        superbomb::PluginPlay::signIn();
//    }
    
    signIn();
}

void PluginPlay::signIn(bool showLoginUI) {
    
    [GameCenterHelper signIn];
}

void PluginPlay::signOut() {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper signOut];
}

bool PluginPlay::isSignedIn() {
    
    return [GameCenterHelper isSignedIn];
}

string PluginPlay::getPlayerId() {

    if( !isSignedIn() ) {
        return "";
    }
    
    return [[GameCenterHelper getPlayerId] UTF8String];
}

void PluginPlay::showAllLeaderboards() {

    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper showAllLeaderboards];
}

void PluginPlay::showLeaderboard(const string &leaderboardName) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper showLeaderboard:NS_STRING(leaderboardName.c_str())];
}

void PluginPlay::submitScore(const string &leaderboardName, long score) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper submitScore:NS_STRING(leaderboardName.c_str()) :score];
}

void PluginPlay::showAchievements() {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper showAchievements];
}

void PluginPlay::loadAchievements(bool forceReload) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper loadAchievements:forceReload];
}

void PluginPlay::unlockAchievement(const string &achievementName) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper unlockAchievement:NS_STRING(achievementName.c_str())];
}

void PluginPlay::incrementAchievement(const string &achievementName, int increment) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper incrementAchievement:NS_STRING(achievementName.c_str()) :increment];
}

void PluginPlay::reveal(const string &achievementName) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper reveal:NS_STRING(achievementName.c_str())];
}

void PluginPlay::setSteps(const string &achievementName, double steps) {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper setSteps:NS_STRING(achievementName.c_str()) :steps];
}

void PluginPlay::resetAchievements() {
    
    if( !isSignedIn() ) {
        return;
    }
    
    [GameCenterHelper resetAchievements];
}

NS_SB_END;

#endif // CC_PLATFORM_IOS
