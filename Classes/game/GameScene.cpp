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

#include "ui/PausePopup.hpp"
#include "ui/ContinuePopup.hpp"
#include "ui/GameOverPopup.hpp"

#include "RankingManager.hpp"
#include "NewRecordPopup.hpp"
#include "CommonLoadingBar.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const string SCHEDULER_GAME_OVER_DELAY = "SCHEDULER_GAME_OVER_DELAY";

GameScene::GameScene() :
gameMgr(GameManager::getInstance()),
gameView(nullptr) {
}

GameScene::~GameScene() {
}

bool GameScene::init() {
    
    if( !BaseScene::init() ) {
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
    initCommonMenu();
    
    gameMgr->addListener(this);
    
    return true;
}

void GameScene::onEnter() {
    
    BaseScene::onEnter();
    
    // 게임뷰 초기화
    gameView = SceneManager::getGameView();
    gameMgr->onEnterGame(gameView);
}

void GameScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    gameMgr->onGameStart();
}

void GameScene::onExit() {
    
    gameMgr->removeListener(this);
    
    AdsHelper::getInstance()->getEventDispatcher()->removeListener(this);
    
    BaseScene::onExit();
}

void GameScene::reset() {
    
    touchLockNode->setVisible(false);
    commonMenu->getTopMenu()->setVisible(true);
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
    commonMenu->getTopMenu()->setVisible(false);
    
    // 다음 단계 지연
    scheduleOnce([=](float dt) {
        
        bool adsLoaded = (AdsHelper::getInstance()->isInterstitialLoaded() ||
                          AdsHelper::getInstance()->isRewardedVideoLoaded());
        
        // 이어하기
        if( adsLoaded &&
            gameMgr->getScore() >= CONTINUE_CONDITION_SCORE &&
            gameMgr->getContinueCount() == 0 ) {
            
            showContinuePopup();
        }
        // 게임 오버
        else {
            gameMgr->onGameOver();
        }
    }, GAME_OVER_DELAY, SCHEDULER_GAME_OVER_DELAY);
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
    
    const int ranking = gameMgr->getRanking();
    
    // 신기록 달성
    if( ranking != INVALID_RANKING ) {
        showNewRecordPopup(ranking, gameMgr->getScore());
    }
    // 게임 오버
    else {
        showGameOverPopup();
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
    
    // 메뉴 클릭 리스너
    popup->setOnClickMenuListener([=](PausePopup::Tag tag) {
        
        switch( tag ) {
            // resume
            case PausePopup::Tag::RESUME: {
                popup->dismissWithAction([=]() {
                    gameMgr->onGameResume();
                });
                
            } break;
                
            // main
            case PausePopup::Tag::MAIN: {
                popup->dismissWithAction([=]() {
                    this->replaceMain();
                });
                
            } break;
                
            // remove ads
            case PausePopup::Tag::REMOVE_ADS: {
                // TODO:
                // popup->dismissWithAction();
                
            } break;
                
            default: break;
        }
    });

    SceneManager::getScene()->addChild(popup, PopupZOrder::MIDDLE);
}

/**
 * 이어하기 팝업 노출
 */
void GameScene::showContinuePopup() {
    
    auto popup = ContinuePopup::create();
    popup->setTag(Tag::POPUP_CONTINUE);
    SceneManager::getScene()->addChild(popup, PopupZOrder::MIDDLE);
    
    // 팝업 종료
    popup->setOnDismissListener([=](Node*){
        
    });
    
    // 비디오 클릭
    popup->setOnVideoListener([=]() {
        
        auto loadingBar = CommonLoadingBar::create();
//        loadingBar->setUIDelay(0.1f);
        loadingBar->show();
        
        // 비디오 광고
        auto adsHelper = AdsHelper::getInstance();
        
        if( adsHelper->isRewardedVideoLoaded() ) {
            CCLOG("Continue showRewardedVideo");

            auto listener = RewardedVideoAdListener::create();
            listener->setTarget(this);
            listener->onRewarded = [=](string type, int amount) {
                // continue
                gameMgr->onContinue();
            };
            listener->onAdClosed = [=]() {
                // 보상 받지 않음, game over
                if( !listener->isRewarded() ) {
                    gameMgr->onGameOver();
                }
                
                loadingBar->dismissWithDelay(0);
            };

            AdsHelper::getInstance()->showRewardedVideo(listener);
        }
        // 전면 광고
        else if( adsHelper->isInterstitialLoaded() ) {
            CCLOG("Continue showInterstitial");
            
            auto listener = AdListener::create(AdType::INTERSTITIAL);
            listener->setTarget(this);
            listener->onAdClosed = [=]() {
                // continue
                gameMgr->onContinue();
                loadingBar->dismissWithDelay(0);
            };
            
            AdsHelper::getInstance()->showInterstitial(listener);
        }
        
        // TODO: ADS
        /*
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
         */
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
    
    // 레코드 완료 리스너
    popup->setOnRecordCompletedListener([=](RankingRecord record) {
        
        // 신기록 등록
        RankingManager::setRecord(record);
    });
    
    // 팝업 이벤트 리스너
    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {

        // 팝업 퇴장 연출 종료 후 게임 오버 팝업 등장
        if( eventType == PopupEventType::EXIT_ACTION_FINISHED ) {
            this->showGameOverPopup();
        }
    });
    
    SceneManager::getScene()->addChild(popup, PopupZOrder::BOTTOM);
}

/**
 * 게임 오버 노출
 */
void GameScene::showGameOverPopup() {
 
    // 공통 메뉴
    commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, false);
    commonMenu->openMenu();
    
    // 게임 오버 팝업
    auto popup = GameOverPopup::create(gameMgr->getScore());
    popup->setTag(Tag::POPUP_GAME_OVER);
    SceneManager::getScene()->addChild(popup, PopupZOrder::BOTTOM);
}

/**
 * 버튼 클릭
 */
void GameScene::onClick(Node *sender) {
}

/**
 * 상단 메뉴 클릭
 */
bool GameScene::onClickTopMenu(TopMenu::Tag tag) {
    
    switch( tag ) {
        // 닫기
        case TopMenu::Tag::BACK:
        case TopMenu::Tag::CLOSE: {
            auto popup = PopupManager::getInstance()->getFrontPopup();
            
            // 일시 정지 팝업
            if( popup->getType() == BasePopup::Type::PAUSE ) {
                auto pausePopup = dynamic_cast<PausePopup*>(popup);
                pausePopup->performListener(PausePopup::Tag::RESUME);

                return true;
            }
            
        } return false;
            
        // 일시정지
        case TopMenu::Tag::PAUSE: {
            showPausePopup();
            
        } return true;
            
        default:
            break;
    }
    
    return false;
}

/**
 * 하단 메뉴 클릭
 */
bool GameScene::onClickBottomMenu(BottomMenu::Tag tag) {
    
    switch( tag ) {
        // 게임 재시작
        case BottomMenu::Tag::START: {
            // gameMgr->onGameRestart();
            gameMgr->onExitGame();
            SceneManager::getInstance()->replace(SceneType::GAME);
            
            /*
            // 팝업 퇴장
            // 게임 오버 창 제거
            auto popup = PopupManager::getInstance()->getPopup(BasePopup::Type::GAME_OVER);
            if( popup ) {
                popup->dismissWithAction();
            }
            
            // 공통 메뉴 퇴장
            commonMenu->closeMenu();
             */
            
        } return true;
            
        // 랭킹
        case BottomMenu::Tag::RANKING_LOCAL: {
            // Step 1. 게임 오버 팝업 퇴장
            auto gameOverPopup = PopupManager::getInstance()->getPopup(BasePopup::Type::GAME_OVER);
            gameOverPopup->runExitAction([=]() {
                
                // Step 2. 랭킹 팝업 등장
//                commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::BACK, 0);
//                commonMenu->getTopMenu()->openMenu(RankingPopup::SLIDE_IN_DURATION);
                
                commonMenu->showRankingPopup([=](Node *sender, PopupEventType eventType) {
                    
                    // 랭킹 팝업 퇴장 시작
                    if( eventType == PopupEventType::EXIT_ACTION ) {
//                        commonMenu->getTopMenu()->closeMenu(RankingPopup::SLIDE_OUT_DURATION);
                    }
                    // 랭킹 퇴장 후 게임 오버 재등장
                    else if( eventType == PopupEventType::EXIT_ACTION_FINISHED ) {
//                        commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, 0);
//                        commonMenu->getTopMenu()->openMenu(GameOverPopup::SLIDE_IN_DURATION);
                        
                        gameOverPopup->runEnterAction();
                    }
                });
            });
            
            commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::BACK);
            
        } return true;
            
        default:
            break;
    }
    
    return false;
}

/**
 * 배경 초기화
 */
void GameScene::initBg() {
    
    touchLockNode = SBNodeUtils::createTouchNode();
    touchLockNode->setVisible(false);
    addChild(touchLockNode, SBZOrder::MIDDLE);
}

/**
 * 메뉴 초기화
 */
void GameScene::initCommonMenu() {
    
    BaseScene::initCommonMenu();
    
    commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::PAUSE, 0);
    commonMenu->getBottomMenu()->setVisible(false);
    
    commonMenu->setOnClickTopMenuListener(CC_CALLBACK_1(GameScene::onClickTopMenu, this));
    commonMenu->setOnClickBottomMenuListener(CC_CALLBACK_1(GameScene::onClickBottomMenu, this));
}


