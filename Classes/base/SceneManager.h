//
//  SceneManager.hpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#ifndef SceneManager_h
#define SceneManager_h

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

enum class SceneType {
    NONE,
    SPLASH,
    MAIN,
    GAME,
};

class SceneManager : public cocos2d::Ref {
public:
    static SceneManager* getInstance();
    static void destroyInstance();
    
    ~SceneManager();
    
private:
    SceneManager();
    
    cocos2d::Scene* createScene(SceneType type);
    
public:
    void replaceScene(SceneType type, std::function<cocos2d::Scene*()> createSceneFunc);
    void replaceScene(SceneType type);
  
    bool onBackKeyReleased();
    
private:
    CC_SYNTHESIZE_READONLY(SceneType, currentSceneType, SceneType);
    CC_SYNTHESIZE_READONLY(cocos2d::Scene*, currentScene, Scene);
    
    bool isRunningReplaceScene;
};

#endif /* SceneManager_h */
