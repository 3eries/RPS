//
//  SBDirector.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef SBDirector_hpp
#define SBDirector_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "SBMacros.h"
#include "SBTypes.hpp"

class SBDirector {
public:
    static SBDirector* getInstance();
    static void destroyInstance();
    ~SBDirector();
    
private:
    SBDirector();
    
public:
    static void init();
    static void end();
    
    static int           getRunCount();
    static bool          isFirstRun();
    static bool          isTodayFirstRun();
    static time_t        getFirstRunTime();
    static time_t        getLastRunTime();
    static std::string   getTodayDate();
    
    static void postDelayed(cocos2d::Node *handler, SBCallback runnable,
                            float delay, bool isScreenTouchLocked = false);
    
public:
    static bool  isPhoneResolution();
    static bool  isPadResolution();
    static bool  isIPad();
    
    static float getScreenRatio();
    
private:
    bool todayFirstRun;
    
    cocos2d::EventListenerTouchOneByOne *touchListener;
    SB_SYNTHESIZE_BOOL(screenTouchLocked, ScreenTouchLocked);   // 화면 터치 잠금 여부
};

#endif /* SBDirector_hpp */
