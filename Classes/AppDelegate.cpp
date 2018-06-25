#include "AppDelegate.h"

#include "superbomb.h"

#include "PatchManager.hpp"
#include "GameConfiguration.hpp"
#include "SceneManager.h"

#include "game/GameManager.hpp"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(720, 1280);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
    GameManager::destroyInstance();
    GameConfiguration::destroyInstance();
    PatchManager::destroyInstance();
    SceneManager::destroyInstance();
    SBDirector::end();
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    
    if(!glview) {
        glview = GLViewImpl::create("RSP");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    ResolutionPolicy::EXACT_FIT);

    register_all_packages();
    
    // 초기화
    
    SBDirector::init();
    PatchManager::getInstance()->init();
//    GameConfiguration::getInstance()->init();

    // run
    SceneManager::getInstance()->replace(SceneType::SPLASH);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    SBAudioEngine::pauseAll();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    SBAudioEngine::resumeAll();
}
