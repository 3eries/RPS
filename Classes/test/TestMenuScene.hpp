//
//  TestMenuScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef TestMenuScene_hpp
#define TestMenuScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseTestScene.hpp"

class TestMenuScene : public BaseTestScene {
public:
    CREATE_FUNC(TestMenuScene);
    ~TestMenuScene();
    
private:
    TestMenuScene();
    
    bool init() override;
};

#endif /* TestMenuScene_hpp */
