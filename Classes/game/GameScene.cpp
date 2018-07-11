//
//  GameScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "GameScene.hpp"

#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "UIHelper.hpp"

#include "GameDefine.h"
#include "GameView.hpp"

#include "../main/MainMenu.hpp"

#include "ui/PausePopup.hpp"
#include "ui/ContinuePopup.hpp"
#include "ui/GameOverPopup.hpp"

#include "RankingManager.hpp"
#include "NewRecordPopup.hpp"

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
            
            if( SceneManager::getInstance()->onBackKeyReleased() ) {
                return;
            }
            
            if( gameMgr->isPreGameOver() || gameMgr->isGameOver() ) {
                return;
            }
            
            // 일시 정지 팝업 처리
            auto pausePopup = PopupManager::getInstance()->getPopup(BasePopup::Type::PAUSE);
            
            // 팝업 제거
            if( pausePopup ) {
                gameMgr->onGameResume();
                pausePopup->dismissWithAction();
            }
            // 팝업 생성
            else if( PopupManager::getInstance()->getPopupCount() == 0 ) {
                // TODO: 효과음 ?
                this->showPausePopup();
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
    
    const int score = gameMgr->getScore();
    const int ranking = RankingManager::getNewRanking(score);
    
    // 신기록 달성
    if( score > 0 && ranking != INVALID_RANKING ) {
        showNewRecordPopup(ranking, score);
    }
    // 게임 오버
    else {
        showGameOver();
    }
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
    popup->setOnClickMenuListener([=](PausePopup::Tag tag) {
        
        switch( tag ) {
            // resume
            case PausePopup::Tag::RESUME: {
                gameMgr->onGameResume();
                popup->dismissWithAction();
                
            } break;
                
            // main
            case PausePopup::Tag::MAIN: {
                this->replaceMain();
                popup->dismissWithAction();
                
            } break;
                
            // remove ads
            case PausePopup::Tag::REMOVE_ADS: {
                // TODO:
                // popup->dismissWithAction();
                
            } break;
                
            default: break;
        }
    });
    SceneManager::getScene()->addChild(popup, POPUP_ZORDER);
}

/**
 * 이어하기 팝업 노출
 */
void GameScene::showContinuePopup() {
    
    auto popup = ContinuePopup::create();
    popup->setTag(Tag::POPUP_CONTINUE);
    SceneManager::getScene()->addChild(popup, POPUP_ZORDER);
    
    // 팝업 종료
    popup->setOnDismissListener([=](Node*){
        
    });
    
    // 비디오 클릭
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
    
    // 타임 오버
    popup->setOnTimeOutListener([=]() {
        gameMgr->onGameOver();
    });
}

/**
 * 신기록 달성 팝업 노출
 */
void GameScene::showNewRecordPopup(int ranking, int score) {
    
    auto popup = NewRecordPopup::create(ranking, score);
    SceneManager::getScene()->addChild(popup, POPUP_ZORDER);
    
    popup->setOnRecordCompletedListener([=](RankingRecord record) {
        
        // 신기록 등록
        RankingManager::setRecord(record);
    });
    
    // 팝업 종료 시 게임 오버 노출
    popup->setOnExitActionListener([=]() {
        
        popup->setLocalZOrder(POPUP_ZORDER+1);
        this->showGameOver();
    });
    
    popup->setOnDismissListener([=](Node*) {
    });
    
    popup->runEnterAction();
}

/**
 * 게임 오버 노출
 */
void GameScene::showGameOver() {
 
    // 메인 메뉴
    mainMenu->openMenu();
    
    // 팝업
    auto popup = GameOverPopup::create(gameMgr->getScore());
    popup->setTag(Tag::POPUP_GAME_OVER);
    
//    popup->setOnClickMenuListener([=](GameOverPopup::MenuType type) {
//
//        switch( type ) {
//            // restart
//            case GameOverPopup::MenuType::RESTART: {
//                gameMgr->onGameRestart();
//                popup->removeFromParent();
//
//            } break;
//
//            // home
//            case GameOverPopup::MenuType::HOME: {
//                this->replaceMain();
//                popup->removeFromParent();
//
//            } break;
//
//            default:
//                break;
//        }
//    });
    SceneManager::getScene()->addChild(popup, POPUP_ZORDER);
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
    addChild(banner, SBZOrder::TOP);
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
    pauseBtn->setAnchorPoint(ANCHOR_MR);
    pauseBtn->setPosition(Vec2TR(-20, TOP_MENU_MARGIN_Y));
    menuLayer->addChild(pauseBtn);
    
    pauseBtn->setOnClickListener(CC_CALLBACK_1(GameScene::onClick, this));
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        pauseBtn->setPositionY(Vec2TR(0, TOP_MENU_MARGIN_Y_BANNER).y);
    }
    
    // 메인 메뉴
    mainMenu = MainMenu::create();
    mainMenu->setOnClickListener([=](MainMenu::Tag tag) -> bool {
        
        switch( tag ) {
            case MainMenu::Tag::START: {
                // gameMgr->onGameRestart();
                gameMgr->onExitGame();
                SceneManager::getInstance()->replace(SceneType::GAME);
                
                // 게임 오버 창 제거
                auto popup = PopupManager::getInstance()->getPopup(BasePopup::Type::GAME_OVER);
                if( popup ) {
                    popup->dismissWithAction();
                }
                
                // 메인 메뉴 퇴장
                mainMenu->closeMenu();
                
            } return true;
                
            default:
                break;
        }
        
        return false;
    });
    mainMenu->setVisible(false);
    addChild(mainMenu, POPUP_ZORDER+1);
}


