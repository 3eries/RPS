//
//  SBTypes.hpp
//
//  Created by hwangseongmin on 2016. 11. 25..
//
//

#ifndef SBTypes_hpp
#define SBTypes_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "SBMacros.h"

NS_SB_BEGIN

typedef unsigned long long int UInt64;
typedef long long int Int64;

typedef long double BigFloat;

NS_SB_END

typedef std::vector<int>            IntList;
typedef std::vector<std::string>    StringList;

typedef enum {
    BOTTOM = 100,
    MIDDLE = 1000,
    TOP    = 10000,
} SBZOrder;

typedef std::function<void()>                           SBCallback;
typedef std::function<void(cocos2d::Ref*)>              SBCallbackRef;
typedef std::function<void(cocos2d::Node*)>             SBCallbackNode;
typedef std::function<void(cocos2d::Node*, void*)>      SBCallbackND;
typedef std::function<void(cocos2d::Ref*, void*)>       SBCallbackObject;
typedef std::function<void(float)>                      SBCallbackSchedule;

enum class SBTouchEventType {
    BEGAN,
    MOVED,
    ENDED,
    CANCELED,
};

class SBNodeListener {
public:
    virtual bool onBackKeyReleased() { return true; }
    
    virtual void onClick(cocos2d::Node *sender) {}
    virtual void onTouch(cocos2d::Node *sender,
                         cocos2d::Touch *touch, SBTouchEventType eventType) {}
};

#endif /* SBTypes_hpp */
