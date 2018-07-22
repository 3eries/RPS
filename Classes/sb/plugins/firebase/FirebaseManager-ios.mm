//
//  FirebaseManager-ios.m
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "FirebaseManager.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import <Foundation/Foundation.h>
#import <Firebase/Firebase.h>

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

void FirebaseManager::init() {
    
    [FIRApp configure];
}

NS_SB_END;

#endif // CC_PLATFORM_IOS
