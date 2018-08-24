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

#include "BaseScene.hpp"

class GameView;

enum class SceneType {
    NONE,
    SPLASH,
    MAIN,
    GAME,
};

class SceneManager : public cocos2d::Ref {
public:
    static const float REPLACE_DURATION_SPLASH_TO_MAIN;
    static const float REPLACE_DURATION_MAIN;
    static const float REPLACE_DURATION_GAME;
    
public:
    static SceneManager* getInstance();
    static void destroyInstance();
    
    ~SceneManager();
    
private:
    SceneManager();
    
    BaseScene* createScene(SceneType type);
    void createGameView();
    
public:
    void replace(SceneType type, std::function<BaseScene*()> createSceneFunc);
    void replace(SceneType type);
    
    bool onBackKeyReleased();
    
// getter
public:
    static SceneType     getSceneType();
    static bool          isGameScene();
    static BaseScene*    getScene();
    static CommonMenu*   getCommonMenu();
    static GameView*     getGameView();
    
private:
    SceneType sceneType;
    BaseScene *scene;
    GameView *gameView;
    
    bool isRunningReplaceScene;
};

#endif /* SceneManager_h */
