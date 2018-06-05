//
//  SplashScene.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "SplashScene.hpp"

#include <spine/spine-cocos2dx.h>

#include "RSP.h"
#include "ResourceHelper.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace spine;
using namespace std;

static const float  CHECK_REPLACE_MAIN_SCENE_INTERVAL   = 1.0f;
static const string CHECK_REPLACE_MAIN_SCENE_SCHEDULER  = "CHECK_REPLACE_MAIN_SCENE_SCHEDULER";

static const float  LAUNCH_IMAGE_DURATION               = 3.0f;
static const string LAUNCH_IMAGE_SCHEDULER              = "LAUNCH_IMAGE_SCHEDULER";

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
    
    // Main으로 전환
    SceneManager::getInstance()->replaceScene(SceneType::MAIN);
}

/**
 * UI 초기화
 */
void SplashScene::initLaunchImage() {
    
    addChild(LayerColor::create(Color4B::BLACK));
    
    // first launch image
//    auto img = Sprite::create(DIR_IMG_SPLASH + "logo_3.jpg");
//    img->setAnchorPoint(ANCHOR_M);
//    img->setPosition(Vec2MC(0,0));
//    addChild(img);

    // 애니메이션
    string jsonFile  = DIR_ANIM + "3eries_logo.json";
    string atlasFile = SBStringUtils::replaceAll(jsonFile, ".json", ".atlas");
    
    auto anim = SkeletonAnimation::createWithJsonFile(jsonFile, atlasFile);
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    anim->setAnimation(0, "run", false);
    addChild(anim);
    
    /*
    auto titleLabel = Label::createWithTTF("3eries", FONT_RETRO, 50);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(0,0));
    addChild(titleLabel);
    */
    
    // title action
    // blink
    /*
    auto delay = DelayTime::create(1);
    auto blink = Blink::create(0.9f, 4);
    auto seq = Sequence::create(delay, blink, nullptr);
    
    titleLabel->runAction(seq);
    */
    
    // fade in
    /*
    titleLabel->setOpacity(0);
    
    auto fadeIn = FadeIn::create(1.0f);
    auto blink = Blink::create(1.0f, 3);
    titleLabel->runAction(Sequence::create(fadeIn, blink, nullptr));
    */
    
    /*
    img->setOpacity(0);
    
    auto fadeIn = FadeIn::create(1.0f);
    img->runAction(fadeIn);
     */
    
    // scheduler
    scheduleOnce([=](float dt) {
        this->launchImageFinished();
    }, LAUNCH_IMAGE_DURATION, LAUNCH_IMAGE_SCHEDULER);
}

/**
 * 런치 이미지 종료
 */
void SplashScene::launchImageFinished() {
    
    if( !isLaunchImageFinished ) {
        isLaunchImageFinished = true;
    }
}

/**
 * 로그인
 */
void SplashScene::login() {
    
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


