//
//  GameView.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameView.hpp"

#include <spine/spine-cocos2dx.h>

#include "SceneManager.h"
#include "User.hpp"
#include "UIHelper.hpp"

#include "GameDefine.h"

#include "object/RSPBlockLayer.hpp"
#include "object/RSPBlock.hpp"
#include "object/RSPButtonLayer.hpp"
#include "object/RSPButton.hpp"
#include "object/Man.hpp"
#include "object/TimeBar.hpp"

#include "ui/GameOverPopup.hpp"
#include "ui/PausePopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const string SCHEDULER_DRAW_DELAY = "SCHEDULER_DRAW_DELAY";

GameView::GameView() :
gameMgr(GameManager::getInstance()),
hitCount(0) {
}

GameView::~GameView() {
    
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    gameMgr->addListener(this);
    gameMgr->onEnterGame(this);
    
    initBg();
    initBlock();
    initRSPButton();
    initMan();
    initTimeBar();
    initLabels();
    initMenu();
    
    ViewManager::getInstance()->addListener(this);
    
    return true;
}

void GameView::onEnter() {
    
    Node::onEnter();
    
    // drop 연출
    /*
    const float POS_Y = blockLayer->getPositionY();
    blockLayer->setPositionY(POS_Y + blockLayer->getContentSize().height);
    
    auto move = MoveTo::create(0.3f, Vec2(blockLayer->getPositionX(), POS_Y));
    blockLayer->runAction(move);
    */
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    CCLOG("onEnterTrans");
    
    auto cloud = getChildByTag<SkeletonAnimation*>(Tag::CLOUD);
    cloud->runAction(FadeIn::create(1.0f));
}

void GameView::onExit() {
    
    ViewManager::getInstance()->removeListener(this);
    gameMgr->removeListener(this);
    
    Node::onExit();
}

/**
 * 뷰 타입 변경
 */
void GameView::onViewChanged(ViewType viewType) {
    
    auto setGameNodeVisible = [=](bool visible) {
        for( auto n : gameNodes ) {
            n->setVisible(visible);
        }
    };
    
    switch( viewType ) {
        case ViewType::MAIN: {
            setGameNodeVisible(false);
        } break;
            
        case ViewType::GAME: {
            setGameNodeVisible(true);
            
            gameMgr->onGameStart();
            
        } break;
            
        default:
            break;
    }
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
    
    // bgm
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        timeBar->setPosition(Vec2TC(0, -BANNER_HEIGHT-10));
        
        auto menuLayer = getChildByTag(Tag::LAYER_MENU);
        menuLayer->getChildByTag(Tag::BTN_PAUSE)->setPosition(Vec2TR(-10, -BANNER_HEIGHT-10));
    }
    
    // 카운트 초기화
    hitCount = 0;
    levelLabel->setString("");
    scoreLabel->setString("0");
    
    showLevelLabel();
}

/**
 * 게임 재시작
 */
void GameView::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void GameView::onGamePause() {
    
    SBNodeUtils::recursivePause(this);
}

/**
 * 게임 재게
 */
void GameView::onGameResume() {
    
    SBNodeUtils::recursiveResume(this);
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
    
    showGameOver();
}

/**
 * 게임 모드 전환
 */
void GameView::onGameModeChanged(GameMode mode) {
    
    feverModeBg->stopAllActions();
    feverModeBg->setVisible(mode == GameMode::FEVER);
    
    switch( mode ) {
        case GameMode::NORMAL: {
            SBAudioEngine::playBGM(SOUND_BGM_GAME);
        } break;
            
        case GameMode::FEVER: {
            SBAudioEngine::playBGM(SOUND_BGM_FEVER);
        } break;
    }
}

/**
 * 피버 모드 종료 전
 */
void GameView::onPreFeverModeEnd() {
    
    auto blink = Blink::create(FEVER_END_ALERT_TIME, 4);
    feverModeBg->runAction(blink);
}

/**
 * 스코어 업데이트
 */
void GameView::updateScore() {
    
    auto saveLevelInfo = gameMgr->getLevelInfo();
    
    gameMgr->setScore(hitCount);
    scoreLabel->setString(TO_STRING(gameMgr->getScore()));
    
    // 레벨 변경된 경우, 레벨 텍스트 연출
    bool isLevelChanged = (saveLevelInfo.level != gameMgr->getLevelInfo().level);
    
    if( isLevelChanged ) {
        showLevelLabel();
    }
}

/**
 * RSP 버튼 클릭
 */
void GameView::onClickNormalButton(RSPType type) {
    
    auto block = blockLayer->getFirstBlock();
    auto result = getResult(type, block->getType());
    
    man->showdown(result, type, block->getType());
    
    switch( result ) {
        case RSPResult::WIN:    this->hitBlock(block, type);      break;
        case RSPResult::LOSE:   this->misBlock(block);            break;
        case RSPResult::DRAW:   this->drawBlock(block);           break;
        default: break;
    }
    
    // 타임바 실행
    if( !timeBar->isStarted() && result != RSPResult::LOSE ) {
        timeBar->startTimeBar();
    }
}

/**
 * 피버 버튼 클릭
 */
void GameView::onClickFeverButton(int i) {
    
    auto block = blockLayer->getFirstBlock();
    man->rockNroll((i == 0) ? Man::Position::LEFT : Man::Position::RIGHT);
    
    hitBlock(block, RSPType::ROCK_N_ROLL);
}

/**
 *  블럭 히트다 히트~!
 */
void GameView::hitBlock(RSPBlock *hitBlock, RSPType btnType) {
    
    CCLOG("GameView::hitBlock gameMode: %d", (int)gameMgr->getGameMode());
    
    blockLayer->hitBlock(hitBlock, btnType, man->getManPosition());
    
    // hit 카운트 증가
    ++hitCount;
    updateScore();
    
    // 시간 증가
    timeBar->increaseTimePoint(gameMgr->getConfig()->getTimeInfo().increasePointPerHit);
    
    // 다음 블럭 타입에 따라 하단 버튼 모드를 변경
    auto nextBlock = blockLayer->getFirstBlock();
    
    if( nextBlock->getType() == RSPType::ROCK_N_ROLL ) {
        buttonLayer->switchButton(GameMode::FEVER);
    } else {
        buttonLayer->switchButton(GameMode::NORMAL);
    }
}

/**
 *  블럭 틀렸다잉
 */
void GameView::misBlock(RSPBlock *block) {
 
    blockLayer->misBlock(block);
    
    gameMgr->onGameOver();
}

/**
 * 비겼당
 */
void GameView::drawBlock(RSPBlock *block) {
    
    blockLayer->drawBlock(block);
    
    // 버튼 터치 지연
    buttonLayer->setButtonTouchEnabled(false);
    
    scheduleOnce([=](float dt) {
        buttonLayer->setButtonTouchEnabled(true);
    }, gameMgr->getConfig()->getTimeInfo().drawDelay, SCHEDULER_DRAW_DELAY);
}

/**
 * Level 라벨 노출
 */
void GameView::showLevelLabel() {
    
    levelLabel->setVisible(true);
    
    levelLabel->stopAllActions();
    levelLabel->setString(STR_FORMAT("LEVEL %d", gameMgr->getLevelInfo().level));
    
    const Vec2 topPos    = Vec2TC(0, levelLabel->getContentSize().height);
    const Vec2 originPos = Vec2MC(0, 415);
    levelLabel->setPosition(topPos);
    
    // move
    auto move1 = MoveTo::create(0.2f, originPos);
    auto move2 = MoveTo::create(0.2f, topPos);
    auto delay = DelayTime::create(1.0f);
    levelLabel->runAction(Sequence::create(move1, delay, move2, nullptr));
    
    // n초 후 hide
//    auto delay = DelayTime::create(1.0f);
//    auto hide = Hide::create();
//    levelLabel->runAction(Sequence::create(delay, hide, nullptr));
}

/**
 * 일시정지 팝업 노출
 */
void GameView::showPausePopup() {
    
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
 * 일시정지 팝업 제거
 */
void GameView::removePausePopup() {
    
    removeChildByTag(Tag::POPUP_PAUSE);
}

/**
 * 게임 오버 출력
 */
void GameView::showGameOver() {
    
    auto popup = GameOverPopup::create();
    popup->setTag(Tag::POPUP_GAME_OVER);
    popup->setOnClickMenuListener([=](GameOverPopup::MenuType type) {
        
        switch( type ) {
            // restart
            case GameOverPopup::MenuType::RESTART: {
                popup->removeFromParent();
                gameMgr->onGameRestart();
                
            } break;
                
            // home
            case GameOverPopup::MenuType::HOME: {
                popup->removeFromParent();
                this->replaceMain();
                
            } break;
                
            default:
                break;
        }
    });
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
    
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
 * 메인 뷰로 전환
 */
void GameView::replaceMain() {
    
    gameMgr->onExitGame();
    
    ViewManager::getInstance()->replaceMainView();
}

/**
 * 버튼 클릭
 */
void GameView::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        case Tag::BTN_PAUSE: {
            showPausePopup();
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
    
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_bg.png");
    bg->setTag(Tag::BG);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg, (int)ZOrder::BG);
    
    feverModeBg = LayerColor::create(Color4B(255,0,0,255*0.1f));
    feverModeBg->setVisible(false);
    addChild(feverModeBg, (int)ZOrder::BG);
    
    // cloud
    string jsonFile  = DIR_ANIM + "RSP_cloud.json";
    string atlasFile = SBStringUtils::replaceAll(jsonFile, ".json", ".atlas");
    
    auto cloud = SkeletonAnimation::createWithJsonFile(jsonFile, atlasFile);
    cloud->setTag(Tag::CLOUD);
    cloud->setAnchorPoint(Vec2::ZERO);
    cloud->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    cloud->setAnimation(0, "run", true);
    cloud->update(0);
    cloud->setOpacity(0);
    addChild(cloud, (int)ZOrder::BG);
    
    // 바닥 기둥
    auto bottomBg = Sprite::create(DIR_IMG_GAME + "RSP_bg_bottom.png");
    bottomBg->setAnchorPoint(ANCHOR_MB);
    bottomBg->setPosition(Vec2BC(0,0));
    addChild(bottomBg, (int)ZOrder::BG);
}

/**
 * RSP 블럭 초기화
 */
void GameView::initBlock() {
    
    blockLayer = RSPBlockLayer::create();
    addChild(blockLayer, -1);
}

/**
 * RSP 버튼 초기화
 */
void GameView::initRSPButton() {
    
    buttonLayer = RSPButtonLayer::create();
    addChild(buttonLayer);
    
    gameNodes.push_back(buttonLayer);
    
    buttonLayer->setOnNormalButtonClickListener([=](RSPType type) {
        this->onClickNormalButton(type);
    });
    
    buttonLayer->setOnFeverButtonClickListener([=](int i) {
        this->onClickFeverButton(i);
    });
}

/**
 * Man 초기화
 */
void GameView::initMan() {
    
    man = Man::create();
    addChild(man);
}

/**
 * TimeBar 초기화
 */
void GameView::initTimeBar() {
    
    timeBar = TimeBar::create();
    timeBar->setAnchorPoint(ANCHOR_MT);
    timeBar->setPosition(Vec2TC(0, -15));
    addChild(timeBar, SBZOrder::BOTTOM);
    
    gameNodes.push_back(timeBar);
    
    // 배너 광고 아래에 위치
    /*
    if( !User::isOwnRemoveAdsItem() ) {
        timeBar->setPosition(Vec2TC(0, -BANNER_HEIGHT-10));
    }
    */
}

/**
 * 라벨 초기화
 */
void GameView::initLabels() {
    
    levelLabel = Label::createWithTTF("", FONT_RETRO, 75);
    levelLabel->setVisible(false);
    levelLabel->setAnchorPoint(ANCHOR_M);
    levelLabel->setPosition(Vec2MC(0, 415));
    levelLabel->setColor(Color3B(250, 178, 11));
    levelLabel->enableOutline(Color4B(78, 22, 0, 255), 7);
    addChild(levelLabel);
    
    gameNodes.push_back(levelLabel);
    
    scoreLabel = Label::createWithTTF("0", FONT_RETRO, 75);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(0, 300));
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 7);
    addChild(scoreLabel);
    
    gameNodes.push_back(scoreLabel);
}

/**
 * 메뉴 초기화
 */
void GameView::initMenu() {
    
    auto menuLayer = SBNodeUtils::createWinSizeNode();
    menuLayer->setTag(Tag::LAYER_MENU);
    addChild(menuLayer, SBZOrder::BOTTOM);
    
    gameNodes.push_back(menuLayer);
    
    // 일시정지
    auto pauseBtn = SBButton::create(DIR_IMG_GAME + "RSP_btn_pause.png");
    pauseBtn->setTag(Tag::BTN_PAUSE);
    pauseBtn->setAnchorPoint(ANCHOR_TR);
    pauseBtn->setPosition(Vec2TR(-10, -10));
    menuLayer->addChild(pauseBtn);
    
    pauseBtn->setOnClickListener(CC_CALLBACK_1(GameView::onClick, this));
    
    // 배너 광고 아래에 위치
    /*
    if( !User::isOwnRemoveAdsItem() ) {
        pauseBtn->setPosition(Vec2TR(-10, -BANNER_HEIGHT-10));
    }
     */
}
