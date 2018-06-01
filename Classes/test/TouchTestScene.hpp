//
//  TouchTestScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef TouchTestScene_hpp
#define TouchTestScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseTestScene.hpp"

class TouchTestScene : public BaseTestScene {
public:
    CREATE_FUNC(TouchTestScene);
    ~TouchTestScene();
    
private:
    TouchTestScene();
    
    bool init() override;
    
    void touchStart();
    
private:
    int touchCount;
};

#endif /* TouchTestScene_hpp */
