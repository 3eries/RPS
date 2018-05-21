//
//  SBJSON.h
//
//  Created by sm on 15. 05. 13..
//
//

#ifndef __SBLib__SBJSON__
#define __SBLib__SBJSON__

#include <string>

#include "cocos2d.h"
#include "json.h"

class SBJSON {
    
public:
    static std::string toJSONString(cocos2d::Value value);
    static Json::Value toJSONValue(cocos2d::Value value);
    
    static cocos2d::Value parse(const std::string &jsonStr);
    static cocos2d::Value parse(Json::Value jsonValue, bool isJsonValue);
    
    static bool isJSON(const std::string &jsonStr);
    
    static Json::LargestUInt getLargestUInt(cocos2d::Value value);
    
private:
    static const char* skip(const char *in);
};

#endif /* defined(__SBLib__SBJSON__) */
