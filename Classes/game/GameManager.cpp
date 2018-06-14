//
//  GameManager.cpp
//
//  Created by seongmin hwang on 2018. 2. 23..
//

#include "GameManager.hpp"

#include "GameView.hpp"

USING_NS_CC;
using namespace std;

static GameManager *instance = nullptr;
GameManager* GameManager::getInstance() {
    
    if( !instance ) {
        instance = new GameManager();
    }
    
    return instance;
}

void GameManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

GameManager::GameManager() :
config(GameConfiguration::getInstance()),
view(nullptr) {
}

GameManager::~GameManager() {
    
    listeners.clear();
}

/**
 * 매니저 초기화
 */
void GameManager::init() {
    
    reset();
}

void GameManager::reset() {
    
    updateLocked = false;
    gamePaused = false;
    gameOver = false;
    gameMode = GameMode::NORMAL;
    levelInfo = GameConfiguration::getInstance()->getLevelInfo(1);
    score = 0;
}

/**
 * 게임 진입
 */
void GameManager::onEnterGame(GameView *view) {
    
    CCASSERT(view != nullptr, "GameManager::onEnterGame error: invalid view.");
    
    this->view = view;
    
    reset();
}

/**
 * 게임 퇴장
 */
void GameManager::onExitGame() {
    
    onGamePause();
    
    view = nullptr;
    
    listeners.clear();
}

/**
 * 스코어 설정
 */
void GameManager::setScore(int score) {
    
    this->score = score;
    
    // 스코어에 해당하는 레벨 설정
    auto infos = GameConfiguration::getInstance()->getLevelInfos();
    
    for( auto info : infos ) {
        if( this->score >= info.beginRange ) {
            this->levelInfo = info;
        }
    }
    
    CCLOG("GameManager::setScore: %d level: %d", this->score, this->levelInfo.level);
}

void GameManager::addScore(int score) {
    
    setScore(this->score + score);
}

GameView* GameManager::getView() {
    return instance->view;
}

#pragma mark- GameListener

/**
 * 게임 시작
 */
void GameManager::onGameStart() {
    
    updateLocked = false;
    gamePaused = false;
    
    onNormalMode();
    
    for( auto listener : listeners ) {
        listener->onGameStart();
    }
}

/**
 * 게임 재시작
 */
void GameManager::onGameRestart() {
    
    reset();
    
    for( auto listener : listeners ) {
        listener->onGameRestart();
    }
    
    onGameStart();
}

/**
 * 게임 일시정지
 */
void GameManager::onGamePause() {
    
    if( gamePaused ) {
        return;
    }
    
    gamePaused = true;
    updateLocked = true;
    
    for( auto listener : listeners ) {
        listener->onGamePause();
    }
}

/**
 * 게임 재게
 */
void GameManager::onGameResume() {
    
    if( !gamePaused ) {
        return;
    }
    
    gamePaused = false;
    updateLocked = false;
    
    for( auto listener : listeners ) {
        listener->onGameResume();
    }
}

/**
 * 게임 오버 전
 */
void GameManager::onPreGameOver() {
    
    // 전면 광고
//    AdsManager::getInstance()->showInterstitial();
    
    onGamePause();
    
    for( auto listener : listeners ) {
        listener->onPreGameOver();
    }
}

/**
 * 게임 오버
 */
void GameManager::onGameOver() {
    
    if( gameOver ) {
        return;
    }
    
    gameOver = true;
    updateLocked = true;
    
//    onGamePause();
    
    for( auto listener : listeners ) {
        listener->onGameOver();
    }
}

/**
 * 게임 종료
 */
void GameManager::onGameEnd() {

    onGamePause();
    
    for( auto listener : listeners ) {
        listener->onGameEnd();
    }
}

/**
 * 게임 모드 전환
 */
void GameManager::onGameModeChanged(GameMode mode) {
    
    CCLOG("GameManager::onGameModeChanged: %d", (int)mode);
    this->gameMode = mode;
    
    for( auto listener : listeners ) {
        listener->onGameModeChanged(mode);
    }
}

/**
 * 노말 모드로 전환
 */
void GameManager::onNormalMode() {
    
    onGameModeChanged(GameMode::NORMAL);
}

/**
 * 노말 모드로 전환
 */
void GameManager::onFeverMode() {
    
    onGameModeChanged(GameMode::FEVER);
}

/**
 * 피버 모드 종료 전
 */
void GameManager::onPreFeverModeEnd() {
    
    for( auto listener : listeners ) {
        listener->onPreFeverModeEnd();
    }
}

void GameManager::addListener(GameListener *listener) {
    
    CCASSERT(listener != nullptr, "GameManager::addListener error: invalid listener.");
    
    auto it = find(listeners.begin(), listeners.end(), listener);
    
    if( it == listeners.end() ) {
        listeners.push_back(listener);
    }
}

void GameManager::removeListener(GameListener *listener) {
    
    CCASSERT(listener != nullptr, "GameManager::removeListener error: invalid listener.");
    
    auto it = find(listeners.begin(), listeners.end(), listener);
    
    if( it != listeners.end() ) {
        listeners.erase(it);
    }
}



