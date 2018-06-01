//
//  TestHelper.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#ifndef TestHelper_hpp
#define TestHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

enum class SceneType {
    NONE,
    SPLASH,
    MAIN,
    GAME,
};

class TestHelper : public cocos2d::Ref {
public:
    static TestHelper* getInstance();
    static void destroyInstance();
    
    ~TestHelper();
    
private:
    TestHelper();
};

#endif /* TestHelper_hpp */
