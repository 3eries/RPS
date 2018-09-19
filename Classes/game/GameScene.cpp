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
    
    iap::IAPHelper::getInstance()->removeListener(this);
}

bool GameScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    firebase::Analytics::setScreenName(FA_SCREEN_GAME);
    
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
            
            // 일시 정지 팝업 생성
            if( PopupManager::getInstance()->getPopupCount() == 0 ) {
                SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
                
                this->showPausePopup();
            }
        };
        
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    initBg();
    initCommonMenu();
    initBanner();
    
    gameMgr->addListener(this);
    addPopupListener();
    
    // IAP 리스너
    {
        auto onRemoveAds = [=]() {
            // 배너 제거
            removeChildByTag(Tag::BANNER_LOADING);
        };
        
        // purchase listener
        auto purchaseListener = iap::PurchaseListener::create();
        purchaseListener->setForever(true);
        purchaseListener->onRemoveAds = onRemoveAds;
        
        iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
        
        // restore listener
        auto restoreListener = iap::RestoreListener::create();
        restoreListener->setForever(true);
        restoreListener->onRemoveAds = onRemoveAds;
        
        iap::IAPHelper::getInstance()->addListener(this, restoreListener);
    }
    
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
    
    PopupManager::getInstance()->removeListener(this);
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
    
    firebase::Analytics::setScreenName(FA_SCREEN_GAME);
    
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
        
        // 이어하기
        if( gameMgr->isContinuable() ) {
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
    
    firebase::Analytics::setScreenName(FA_SCREEN_GAME_RESULT);
    
    // 신기록 달성
    if( gameMgr->isNewRecord() ) {
        showNewRecordPopup(gameMgr->getRanking(), gameMgr->getScore());
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
    
    removeChildByTag(Tag::BANNER_LOADING);
    
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
                if( iap::IAPHelper::isReady() ) {
                    auto loadingBar = CommonLoadingBar::create();
                    loadingBar->setUIDelay(0.1f);
                    loadingBar->show();
                    
                    auto listener = iap::PurchaseListener::create();
                    listener->setTarget(this);
                    listener->onPurchased = [=](const iap::Item &item) {
                    };
                    listener->onFinished = [=](bool result) {
                        loadingBar->dismissWithDelay(0);
                    };
                    
                    iap::IAPHelper::purchaseRemoveAds(listener);
                }
                
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
    
    bool isAdsLoaded = AdsHelper::getInstance()->isRewardedVideoLoaded();
//    bool isAdsLoaded = false;
    
    auto popup = ContinuePopup::create(isAdsLoaded);
    popup->setTag(Tag::POPUP_CONTINUE);
    SceneManager::getScene()->addChild(popup, PopupZOrder::MIDDLE);
    
    // 팝업 종료
    popup->setOnDismissListener([=](Node*){
        
    });
    
    // 이어하기
    popup->setOnContinueListener([=]() {
        
        auto darkLayer = LayerColor::create(Color4B::BLACK);
        this->addChild(darkLayer, SBZOrder::TOP);
        
        auto continueWithAction = [=](FiniteTimeAction *firstAction) {
            
            SBDirector::getInstance()->setScreenTouchLocked(true);
            
            Vector<FiniteTimeAction*> actions;
            
            if( firstAction ) {
                actions.pushBack(firstAction);
            }
            
            actions.pushBack(CallFunc::create([=]() {
                gameMgr->onContinue();
            }));
            actions.pushBack(DelayTime::create(0.1f));
            actions.pushBack(FadeOut::create(0.4f));
            actions.pushBack(CallFunc::create([=]() {
                SBDirector::getInstance()->setScreenTouchLocked(false);
            }));
            actions.pushBack(RemoveSelf::create());
            
            darkLayer->runAction(Sequence::create(actions));
        };

        // 광고 없음, 무료 이어하기
        if( !isAdsLoaded ) {
            darkLayer->setOpacity(0);
            
            auto fadeIn = FadeIn::create(0.4f);
            auto callFunc = CallFunc::create([=]() {
                popup->dismiss();
            });
            auto firstAction = Sequence::create(fadeIn, callFunc, nullptr);
            continueWithAction(firstAction);
            
            return;
        }
        
        // 광고 있음, 팝업 퇴장 연출 후 광고 재생
        popup->dismissWithAction([=]() {
            
            auto loadingBar = CommonLoadingBar::create();
            loadingBar->setUIDelay(0.1f);
            loadingBar->show();
            
            // 동영상 광고 노출
            CCLOG("Continue showRewardedVideo");
            
            auto listener = RewardedVideoAdListener::create();
            listener->setTarget(this);
            
            // 로딩 실패
            listener->onAdFailedToLoad = [=](int error) {
                
                firebase::Analytics::logEvent("debug_continue_rewarded_ad");
//                darkLayer->removeFromParent();
//                gameMgr->onGameOver();
            };
            // 보상 완료
            listener->onRewarded = [=](string type, int amount) {
            };
            // 광고 open
            listener->onAdOpened = [=]() {
                loadingBar->dismissWithDelay(0);
            };
            // 광고 close
            listener->onAdClosed = [=]() {
                
                if( listener->isRewarded() ) {
                    continueWithAction(nullptr);
                } else {
                    darkLayer->removeFromParent();
                    gameMgr->onGameOver();
                }
            };
            
            AdsHelper::getInstance()->showRewardedVideo(listener);
        });
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
            this->showGameOverPopup([=](Node *sender, PopupEventType eventType) {
                
                if( eventType == PopupEventType::ENTER ) {
                    auto gameOverPopup = (GameOverPopup*)sender;
                    // gameOverPopup->setEnterTimeScale(0.7f);
                }
            });
        }
    });
    
    SceneManager::getScene()->addChild(popup, PopupZOrder::BOTTOM);
}

/**
 * 게임 오버 노출
 */
void GameScene::showGameOverPopup(OnPopupEvent onEventListener) {
 
    // 공통 메뉴
    commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, false);
    commonMenu->openMenu();
    
    // 게임 오버 팝업
    auto popup = GameOverPopup::create(gameMgr->getScore());
    popup->setTag(Tag::POPUP_GAME_OVER);
    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
        
        switch( eventType ) {
            case PopupEventType::ENTER_ACTION_FINISHED: {
                popup->setEnterTimeScale(1);
            } break;
                
            case PopupEventType::EXIT_ACTION_FINISHED: {
                popup->setExitTimeScale(1);
            } break;
                
            default: break;
        }
        
        if( onEventListener ) {
            onEventListener(sender, eventType);
        }
    });
    SceneManager::getScene()->addChild(popup, PopupZOrder::BOTTOM);
}

/**
 * 리뷰 체크
 * 조건 충족 시 리뷰 작성 알림 팝업 노출
 */
void GameScene::checkReview() {
    
    if( gameMgr->isNewRecord()  ||          // 신기록
        gameMgr->getScore() >= 400 ||       // 스코어
        gameMgr->getPlayCount() >= 15 ) {   // 플레이 횟수
        
        User::checkReview(0.5f);
    }
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
            const float POPUP_EFFECT_TIME_SCALE = 0.7f;
            
            // Step 1. 게임 오버 팝업 퇴장
            auto gameOverPopup = PopupManager::getInstance()->getPopup(BasePopup::Type::GAME_OVER);
            gameOverPopup->setExitTimeScale(POPUP_EFFECT_TIME_SCALE);
            gameOverPopup->runExitAction([=]() {
                
                // Step 2. 랭킹 팝업 등장
//                commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::BACK, 0);
//                commonMenu->getTopMenu()->openMenu(RankingPopup::SLIDE_IN_DURATION);
                
                commonMenu->showRankingPopup([=](Node *sender, PopupEventType eventType) {
                    
                    auto rankingPopup = (RankingPopup*)sender;
                    
                    switch( eventType ) {
                        // 랭킹 등장
                        case PopupEventType::ENTER: {
                            rankingPopup->setEnterTimeScale(POPUP_EFFECT_TIME_SCALE);
                            rankingPopup->setExitTimeScale(POPUP_EFFECT_TIME_SCALE);
                        } break;
                    
                        // 랭킹 퇴장 연출 시작
                        case PopupEventType::EXIT_ACTION: {
                            // commonMenu->getTopMenu()->closeMenu(RankingPopup::SLIDE_OUT_DURATION);
                        } break;
                    
                        // 랭킹 퇴장 완료
                        case PopupEventType::EXIT_ACTION_FINISHED: {
                             // 게임 오버 재등장
                            // commonMenu->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, 0);
                            // commonMenu->getTopMenu()->openMenu(GameOverPopup::SLIDE_IN_DURATION);
                            
                            gameOverPopup->setEnterTimeScale(POPUP_EFFECT_TIME_SCALE);
                            gameOverPopup->runEnterAction();
                            
                        } break;
                            
                        default: break;
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
 * 배너 초기화
 */
void GameScene::initBanner() {
    
    // 배너 로딩중 이미지
    // 로딩된 배너 광고의 ZOrder가 높기 때문에 로딩중 이미지를 따로 제거하지 않는다.
    if( User::isOwnRemoveAdsItem() ) {
        return;
    }
    
    const Size bannerSize(SB_WIN_SIZE.width, AdsHelper::getBannerHeight());
    
    auto bannerLayer = Node::create();
    bannerLayer->setTag(Tag::BANNER_LOADING);
    bannerLayer->setAnchorPoint(ANCHOR_MT);
    bannerLayer->setPosition(Vec2TC(0, 0));
    bannerLayer->setContentSize(bannerSize);
    addChild(bannerLayer, INT_MAX);
    
    {
        auto bg = Sprite::create(DIR_IMG_GAME + "RSP_ad_top_bg.png");
        bg->setAnchorPoint(ANCHOR_M);
        bg->setPosition(Vec2MC(bannerSize, 0, 0));
        bg->setScaleX(bannerSize.width / bg->getContentSize().width);
        bg->setScaleY(bannerSize.height / bg->getContentSize().height);
        bannerLayer->addChild(bg);
        
        auto text = Sprite::create(DIR_IMG_GAME + "RSP_ad_top_text.png");
        text->setAnchorPoint(ANCHOR_M);
        text->setPosition(Vec2MC(bannerSize, 0, 0));
        bannerLayer->addChild(text);
    }
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

void GameScene::addPopupListener() {
    
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType eventType) {
        
        auto popup = dynamic_cast<BasePopup*>(sender);
        
        // 게임 오버 등장 연출 완료 후, 전면 광고 노출
        if( popup->getType() == BasePopup::Type::GAME_OVER &&
            eventType == PopupEventType::ENTER_ACTION_FINISHED ) {
            
            CCLOG("User::isOwnRemoveAdsItem: %d AdsHelper::isInterstitialLoaded: %d", User::isOwnRemoveAdsItem(), AdsHelper::isInterstitialLoaded());
            
            // 전면 광고 O, 1초 후 노출
            if( !User::isOwnRemoveAdsItem() &&          // 광고 제거 아이템 없음
                AdsHelper::isInterstitialLoaded() &&    // 광고 로드됨
                gameMgr->getFeverModeCount() > 0 &&     // 피버 모드 횟수
               !gameMgr->isInterstitialAdOpened() ) {
                
                SBDirector::postDelayed(this, [=]() {
                    auto listener = AdListener::create(AdType::INTERSTITIAL);
                    listener->setTarget(this);
                    listener->onAdOpened = [=]() {
                        gameMgr->setInterstitialAdOpened(true);
                    };
                    listener->onAdClosed = [=]() {
                        this->checkReview();
                    };
                    AdsHelper::getInstance()->showInterstitial(listener);
                }, 1.0f, true);
            }
            // 전면 광고 X
            else {
                this->checkReview();
            }
        }
    };
    
    PopupManager::getInstance()->addListener(listener);
}
