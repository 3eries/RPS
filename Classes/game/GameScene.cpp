//
//  GameScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "GameScene.hpp"

#include "SceneManager.h"
#include "UIHelper.hpp"
#include "GameValue.hpp"

#include "GameLayer.hpp"
#include "ui/GameOverPopup.hpp"
#include "ui/PausePopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

GameScene::GameScene(){
}

GameScene::~GameScene() {
}

bool GameScene::init() {
    
    if( !Scene::init() ) {
        return false;
    }
    
    initBg();
    initGameLayer();
    initMenu();
    
    return true;
}

void GameScene::onEnter() {
    
    Scene::onEnter();
}

void GameScene::onEnterTransitionDidFinish() {
    
    Scene::onEnterTransitionDidFinish();
    
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
}

void GameScene::onExit() {
    
    Scene::onExit();
}

/**
 * 메인 화면으로 이동
 */
void GameScene::replaceMainScene() {
    
    SceneManager::getInstance()->replaceScene(SceneType::MAIN);
}

/**
 * 일시정지 팝업 노출
 */
void GameScene::showPausePopup() {
    
    if( getChildByTag(Tag::POPUP_PAUSE) ) {
        // 팝업이 이미 존재함
        return;
    }

    auto popup = PausePopup::create();
    popup->setTag(Tag::POPUP_PAUSE);
    popup->setOnClickMenuListener([=](PausePopup::MenuType type) {

        switch( type ) {
            // resume
            case PausePopup::MenuType::RESUME: {
//                gameMgr->onGameResume();
                popup->removeFromParent();
            } break;

            // home
            case PausePopup::MenuType::HOME: {
//                gameMgr->onGameOver();
                this->replaceMainScene();
                
                popup->removeFromParent();
                
            } break;

            default:
                break;
        }
    });
    addChild(popup, SBZOrder::TOP);
}

/**
 * 일시정지 팝업 제거
 */
void GameScene::removePausePopup() {
    
    removeChildByTag(Tag::POPUP_PAUSE);
}

/**
 * 게임 오버 출력
 */
void GameScene::showGameOver() {
 
    auto popup = GameOverPopup::create();
    popup->setTag(Tag::POPUP_GAME_OVER);
    popup->setOnClickMenuListener([=](GameOverPopup::MenuType type) {
        
        switch( type ) {
            // restart
            case GameOverPopup::MenuType::RESTART: {
                popup->removeFromParent();
                
                SceneManager::getInstance()->replaceScene(SceneType::GAME);
                
            } break;
                
            // home
            case GameOverPopup::MenuType::HOME: {
                popup->removeFromParent();
                
                this->replaceMainScene();
                
            } break;
                
            default:
                break;
        }
    });
    addChild(popup, SBZOrder::TOP);
    
    // 게임 오버 배경음 재생
    /*
    auto audioId = SBAudioEngine::playBGM(SOUND_GAME_OVER, false);
    
    CCLOG("duration: %f, %f, %f",
          SBAudioEngine::getDuration(SOUND_BGM),
          SBAudioEngine::getDuration(SOUND_GAME_OVER),
          SBAudioEngine::getDuration(audioId));
    
    // 배경음 원상복구
    SBAudioEngine::setOnFinishedListener(audioId, [=](int, const string &) {
        SBAudioEngine::playBGM(SOUND_BGM);
    });
    */
    
    /*
    scheduleOnce([=](float dt) {
        
        SBAudioEngine::playBGM(SOUND_BGM);
        
    }, SBAudioEngine::getDuration(SOUND_GAME_OVER), "GameOverSound");
    */
}

/**
 * 버튼 클릭
 */
void GameScene::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        case Tag::BTN_PAUSE: {
            showPausePopup();
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameScene::initBg() {
    
    // bg
    addChild(LayerColor::create(Color4B::BLACK), -1);
    
    auto bg = Sprite::create(DIR_IMG_MAIN + "main_bg.png");
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg, -1);
}

/**
 * 게임 레이어 초기화
 */
void GameScene::initGameLayer() {
    
    gameLayer = GameLayer::create();
    gameLayer->setOnGameOverListener([=]() {
        this->showGameOver();
    });
    addChild(gameLayer);
}

/**
 * 메뉴 초기화
 */
void GameScene::initMenu() {
    
    auto menuLayer = SBNodeUtils::createWinSizeNode();
    addChild(menuLayer);
    
    // 일시정지
    auto pauseBtn = SBButton::create(DIR_IMG_GAME + "game_btn_pause.png", Size(80,80));
    pauseBtn->setTag(Tag::BTN_PAUSE);
    pauseBtn->setAnchorPoint(ANCHOR_TR);
    pauseBtn->setPosition(Vec2TR(-10, -10));
    menuLayer->addChild(pauseBtn);
    
    pauseBtn->setOnClickListener(CC_CALLBACK_1(GameScene::onClick, this));
    
}


