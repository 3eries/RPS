//
//  GameScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "GameScene.hpp"

#include "User.hpp"
#include "SceneManager.h"
#include "UIHelper.hpp"

#include "GameDefine.h"
#include "GameView.hpp"

#include "ui/PausePopup.hpp"
#include "ui/ContinuePopup.hpp"
#include "ui/GameOverPopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

GameScene::GameScene() :
gameMgr(GameManager::getInstance()),
gameView(nullptr) {
}

GameScene::~GameScene() {
}

bool GameScene::init() {
    
    if( !Scene::init() ) {
        return false;
    }
    
    // back key
    {
        auto listener = EventListenerKeyboard::create();
        listener->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event *event) {
            
            if( keyCode != EventKeyboard::KeyCode::KEY_BACK ) {
                return;
            }
            
            
        };
        
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    initBg();
    initMenu();
    
    gameMgr->addListener(this);
    
    return true;
}

void GameScene::onEnter() {
    
    Scene::onEnter();
    
    // 게임뷰 초기화
    gameView = SceneManager::getGameView();
    gameMgr->onEnterGame(gameView);
}

void GameScene::onEnterTransitionDidFinish() {
    
    Scene::onEnterTransitionDidFinish();
    
    gameMgr->onGameStart();
}

void GameScene::onExit() {
    
    gameMgr->removeListener(this);
    
    Scene::onExit();
}

void GameScene::reset() {
    
    touchLockNode->setVisible(false);
    getChildByTag(Tag::LAYER_MENU)->setVisible(true);
}

/**
 * 게임 시작
 */
void GameScene::onGameStart() {
    
    reset();
}

/**
 * 게임 재시작
 */
void GameScene::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void GameScene::onGamePause() {
    
    touchLockNode->setVisible(true);
}

/**
 * 게임 재게
 */
void GameScene::onGameResume() {
    
    touchLockNode->setVisible(false);
}

/**
 * 게임 오버 전
 */
void GameScene::onPreGameOver() {
    
    touchLockNode->setVisible(true);
    getChildByTag(Tag::LAYER_MENU)->setVisible(false);
    
    // 이어하기
    if( gameMgr->getScore() >= CONTINUE_CONDITION_SCORE && gameMgr->getContinueCount() == 0 ) {
        showContinuePopup();
    }
    // 게임 오버
    else {
        gameMgr->onGameOver();
    }
}

/**
 * 이어하기
 */
void GameScene::onContinue() {
    
    reset();
}

/**
 * 게임 오버
 */
void GameScene::onGameOver() {
    
    showGameOver();
}

/**
 * 메인 화면으로 이동
 */
void GameScene::replaceMain() {
    
    gameMgr->onExitGame();
    
    SceneManager::getInstance()->replace(SceneType::MAIN);
}

/**
 * 일시정지 팝업 노출
 */
void GameScene::showPausePopup() {
    
    if( getChildByTag(Tag::POPUP_PAUSE) ) {
        // 팝업이 이미 존재함
        return;
    }
    
    gameMgr->onGamePause();
    
    auto popup = PausePopup::create();
    popup->setTag(Tag::POPUP_PAUSE);
    popup->setOnClickMenuListener([=](PausePopup::MenuType type) {
        
        switch( type ) {
            // resume
            case PausePopup::MenuType::RESUME: {
                gameMgr->onGameResume();
                popup->removeFromParent();
                
            } break;
                
            // main
            case PausePopup::MenuType::MAIN: {
                this->replaceMain();
                popup->removeFromParent();
                
            } break;
                
            default:
                break;
        }
    });
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
}

/**
 * 이어하기 팝업 노출
 */
void GameScene::showContinuePopup() {
    
    auto popup = ContinuePopup::create();
    popup->setTag(Tag::POPUP_CONTINUE);
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
    
    // onClose
    popup->setOnClosedListener([=]() {
        
    });
    
    // onVideo
    popup->setOnVideoListener([=]() {
        
        // TODO: ADS
        auto bg = SBNodeUtils::createTouchNode(Color4B::BLACK);
        SceneManager::getScene()->addChild(bg, SBZOrder::TOP);
        
        auto label = Label::createWithTTF("Video...", FONT_RETRO, 90);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(0, 0));
        label->setColor(Color3B::WHITE);
        bg->addChild(label);
        
        auto delay = DelayTime::create(3);
        auto callFunc = CallFunc::create([=]() {
            // continue
            gameMgr->onContinue();
        });
        auto remove = RemoveSelf::create();
        bg->runAction(Sequence::create(delay, callFunc, remove, nullptr));
    });
    
    // onTimeOut
    popup->setOnTimeOutListener([=]() {
        gameMgr->onGameOver();
    });
}

/**
 * 게임 오버 노출
 */
void GameScene::showGameOver() {
 
    auto popup = GameOverPopup::create();
    popup->setTag(Tag::POPUP_GAME_OVER);
    popup->setOnClickMenuListener([=](GameOverPopup::MenuType type) {
        
        switch( type ) {
            // restart
            case GameOverPopup::MenuType::RESTART: {
                gameMgr->onGameRestart();
                popup->removeFromParent();
                
            } break;
                
            // home
            case GameOverPopup::MenuType::HOME: {
                this->replaceMain();
                popup->removeFromParent();
                
            } break;
                
            default:
                break;
        }
    });
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
}

/**
 * 일시정지 팝업 제거
 */
void GameScene::removePausePopup() {
    
    removeChildByTag(Tag::POPUP_PAUSE);
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
    
    touchLockNode = SBNodeUtils::createTouchNode();
    touchLockNode->setVisible(false);
    addChild(touchLockNode, SBZOrder::MIDDLE);
    
    // 임시 배너 이미지
    banner = Sprite::create(DIR_IMG_GAME + "RSP_ad_top.png");
    banner->setVisible(!User::isOwnRemoveAdsItem());
    banner->setAnchorPoint(ANCHOR_MT);
    banner->setPosition(Vec2TC(0, 0));
    addChild(banner, SBZOrder::MIDDLE);
}

/**
 * 메뉴 초기화
 */
void GameScene::initMenu() {
    
    auto menuLayer = SBNodeUtils::createZeroSizeNode();
    menuLayer->setTag(Tag::LAYER_MENU);
    addChild(menuLayer, SBZOrder::BOTTOM);
    
    // 일시정지
    auto pauseBtn = SBButton::create(DIR_IMG_GAME + "RSP_btn_pause.png");
    pauseBtn->setTag(Tag::BTN_PAUSE);
    pauseBtn->setAnchorPoint(ANCHOR_TR);
    pauseBtn->setPosition(Vec2TR(-10, -10));
    menuLayer->addChild(pauseBtn);
    
    pauseBtn->setOnClickListener(CC_CALLBACK_1(GameScene::onClick, this));
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        pauseBtn->setPosition(Vec2TR(-10, -BANNER_HEIGHT-10));
    }
}


