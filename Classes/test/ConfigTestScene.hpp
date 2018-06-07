//
//  ConfigTestScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef ConfigTestScene_hpp
#define ConfigTestScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseTestScene.hpp"

class ConfigTestScene : public BaseTestScene {
public:
    CREATE_FUNC(ConfigTestScene);
    ~ConfigTestScene();
    
private:
    ConfigTestScene();
    
    bool init() override;
};

#endif /* ConfigTestScene_hpp */
