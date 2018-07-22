//
//  Analytics-ios.m
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#include "Analytics.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import <Foundation/Foundation.h>
#import <Firebase/Firebase.h>

#include "../../platform/SBPlatformMacros.h"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

namespace firebase {
    
    void Analytics::setScreenName(const string &screen, const string &screenClass) {
        
        [FIRAnalytics setScreenName:NS_STRING(screen.c_str())
                        screenClass:(screenClass != "") ? NS_STRING(screenClass.c_str()) : nil];
    }
    
    void Analytics::logEvent(const string &event, const EventParams &params) {
        
        NSMutableDictionary *nsParams = [NSMutableDictionary dictionary];
        
        for( auto it = params.begin(); it != params.end(); ++it ) {
            NSString *key = NS_STRING(it->first.c_str());
            NSString *value = NS_STRING(it->second.c_str());
            
            [nsParams setValue:value forKey:key];
        }
        
        [FIRAnalytics logEventWithName:NS_STRING(event.c_str())
                            parameters:nsParams];
    }
    
} // namespace firebase

NS_SB_END;

#endif // CC_PLATFORM_IOS
