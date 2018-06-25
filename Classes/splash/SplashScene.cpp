//
//  SplashScene.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "SplashScene.hpp"

#include <spine/spine-cocos2dx.h>

#include "RSP.h"
#include "PatchManager.hpp"
#include "ResourceHelper.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"

#include "LogoView.hpp"

#include "../game/GameManager.hpp"

USING_NS_CC;
using namespace spine;
using namespace std;

static const float  CHECK_REPLACE_MAIN_SCENE_INTERVAL   = 0.05f;
static const string CHECK_REPLACE_MAIN_SCENE_SCHEDULER  = "CHECK_REPLACE_MAIN_SCENE_SCHEDULER";

static const float  LOGIN_TIME_OUT                      = 3.0f;
static const string LOGIN_TIME_OUT_SCHEDULER            = "LOGIN_TIME_OUT_SCHEDULER";

SplashScene::SplashScene() :
isReplacedMainScene(false),
isLaunchImageFinished(false),
isLoginCompleted(false) {
    
}

SplashScene::~SplashScene() {
    
}

bool SplashScene::init() {
    
    if( !Scene::init() ) {
        return false;
    }
    
    initLaunchImage();
    login();
    
    ResourceHelper::preload();
    
    // 메인 화면 전환 체크 스케줄러 실행
    schedule([=](float dt) {
        if( isLaunchImageFinished && isLoginCompleted ) {
            this->replaceMainScene();
        }
    }, CHECK_REPLACE_MAIN_SCENE_INTERVAL, CHECK_REPLACE_MAIN_SCENE_SCHEDULER);
    
    return true;
}

void SplashScene::onEnter() {
    
    Scene::onEnter();
}

void SplashScene::onExit() {
    
//    AdsManager::getInstance()->getEventDispatcher()->removeListener(this);
    
    Scene::onExit();
}

/**
 * 메인화면으로 전환
 */
void SplashScene::replaceMainScene() {
    
    if( isReplacedMainScene ) {
        return;
    }
    
    isReplacedMainScene = true;
    
    // 매니저 초기화
    GameManager::getInstance()->init();
    
    // Main으로 전환
    SceneManager::getInstance()->replace(SceneType::MAIN);
}

/**
 * UI 초기화
 */
void SplashScene::initLaunchImage() {
    
    // addChild(LayerColor::create(Color4B::WHITE));

    // 로고
    auto logoView = LogoView::create();
    addChild(logoView);
    
    logoView->setOnFinishedListener([=]() {
        this->launchImageFinished();
    });
    logoView->run();
}

/**
 * 런치 이미지 종료
 */
void SplashScene::launchImageFinished() {
    
    CCLOG("SplashScene::launchImageFinished");
    
    if( !isLaunchImageFinished ) {
        isLaunchImageFinished = true;
    }
}

/**
 * 로그인
 */
void SplashScene::login() {
    
    auto patchMgr = PatchManager::getInstance();
    patchMgr->setOnAppVersion([=](AppVersion version) {
        
        
    });
    
    patchMgr->setOnGameConfig([=](string version, string json) {
        
    });
    
    patchMgr->patch();
    
    loginCompleted();
}

/**
 * 로그인 완료
 */
void SplashScene::loginCompleted() {
    
    if( !isLoginCompleted ) {
        isLoginCompleted = true;
    }
}


