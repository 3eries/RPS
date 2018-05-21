//
//  SBSystemUtils.h
//  SBLib
//
//  Created by hwangseongmin on 2015. 6. 5..
//
//

#ifndef __SBLib__SBSystemUtils__
#define __SBLib__SBSystemUtils__

#include <stdio.h>

#include "cocos2d.h"

class SBSystemUtils {
public:
    static void exitApp();
    
    static struct tm* getCurrentTime();
    static std::string getCurrentTimeString(const std::string &format);
    static double getCurrentTimeMillis();
    static double getCurrentTimeSeconds();
};

#endif /* defined(__SBLib__SBSystemUtils__) */
