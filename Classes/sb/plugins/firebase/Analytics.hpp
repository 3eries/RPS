//
//  Analytics.hpp
//
//  Created by seongmin hwang on 2018. 7. 21..
//

#ifndef Analytics_hpp
#define Analytics_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../../base/SBMacros.h"
#include "../../base/SBTypes.hpp"

NS_SB_BEGIN;

namespace firebase {

class Analytics {
public:
    typedef std::map<std::string, std::string> EventParams;
    
public:
    static void setScreenName(const std::string &screen, const std::string &screenClass = "");
    static void logEvent(const std::string &event, const EventParams &params = EventParams());
};

} // namespace firebase
    
NS_SB_END;

#endif /* Analytics_hpp */
