//
//  SceneManager.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "SceneManager.h"

#include "../splash/SplashScene.hpp"
#include "../main/MainScene.hpp"
#include "../game/GameScene.hpp"
#include "../game/GameView.hpp"

USING_NS_CC;
using namespace std;

static SceneManager *instance = nullptr;
SceneManager* SceneManager::getInstance() {
    
    if( !instance ) {
        instance = new SceneManager();
    }
    
    return instance;
}

void SceneManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

SceneManager::SceneManager() :
sceneType(SceneType::NONE),
scene(nullptr),
gameView(nullptr),
isRunningReplaceScene(false) {
}

SceneManager::~SceneManager() {
    
    CC_SAFE_RELEASE_NULL(gameView);
}

/**
 * Scene 생성
 */
Scene* SceneManager::createScene(SceneType type) {
    
    switch( type ) {
        case SceneType::SPLASH:          return SplashScene::create();
        case SceneType::MAIN:            return MainScene::create();
        case SceneType::GAME:            return GameScene::create();
        default:
            CCASSERT(false, "SceneManager::createScene error: invalid scene type.");
            break;
    }
    
    return nullptr;
}

void SceneManager::createGameView() {
    
    if( sceneType != SceneType::MAIN && sceneType != SceneType::GAME ) {
        return;
    }
    
    if( gameView ) {
        gameView->removeFromParent();
        CC_SAFE_RELEASE_NULL(gameView);
    }
    
    gameView = GameView::create();
    gameView->retain();
    scene->addChild(gameView, -1);
}

/**
 * Scene 전환
 */
void SceneManager::replace(SceneType type, function<Scene*()> createSceneFunc) {
    
    CCASSERT(type != SceneType::NONE, "SceneManager::replaceScene error: invalid scene type.");
    CCASSERT(createSceneFunc != nullptr, "SceneManager::replaceScene error: create scene function is null.");
    
    if( isRunningReplaceScene ) {
        return;
    }
    
    isRunningReplaceScene = true;
    this->sceneType = type;

    // Scene 생성
    auto scene = createSceneFunc();
    this->scene = scene;
    
    CCASSERT(scene != nullptr, "SceneManager::replaceScene error: invalid scene.");
    
    // 게임뷰 생성
    createGameView();
    
    // Scene 전환 효과
//    auto trans = TransitionCrossFade::create(0.3f, newScene);
//    auto trans = TransitionFade::create(0.3f, newScene);
    
    switch( type ) {
        case SceneType::SPLASH:          break;
        case SceneType::MAIN:            scene = TransitionCrossFade::create(0.7f, scene);     break;
        case SceneType::GAME:            scene = TransitionFade::create(0.7f, scene);          break;
            
        default: break;
    }
    
    // Scene 전환
    Director::getInstance()->replaceScene(scene);
    
    // Scene 전환 중복 방지
    Director::getInstance()->getScheduler()->schedule([=](float dt) {
        this->isRunningReplaceScene = false;
    }, this, 0, 0, 0.1f, false, "SCENE_ON_ENTER");
}

void SceneManager::replace(SceneType type) {
    
    return replace(type, [=]() -> Scene* {
        return this->createScene(type);
    });
}

/**
 * 백키 이벤트 핸들링
 * Scene 공통으로 처리할 로직 구현
 */
bool SceneManager::onBackKeyReleased() {
    
    if( !scene ) {
        return false;
    }
    
    auto children = scene->getChildren();
    
    for( auto child : children ) {
        if( false /*TODO*/) {
            return true;
        }
    }
    
    return false;
}

SceneType SceneManager::getSceneType() {
    return instance->sceneType;
}

Scene* SceneManager::getScene() {
    return instance->scene;
}

GameView* SceneManager::getGameView() {
    return instance->gameView;
}

