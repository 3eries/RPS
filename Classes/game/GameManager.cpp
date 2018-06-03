//
//  GameManager.cpp
//
//  Created by seongmin hwang on 2018. 2. 23..
//

#include "GameManager.hpp"

#include "GameScene.hpp"
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
view(nullptr) {
}

GameManager::~GameManager() {
    
    listeners.clear();
}

/**
 * 매니저 초기화
 */
void GameManager::init() {
}

/**
 * 게임 진입
 */
void GameManager::onEnterGame(GameView *view) {
    
    CCASSERT(view != nullptr, "GameManager::onEnterGame error: invalid view.");
    
    this->view = view;
    
    updateLocked = false;
    gamePaused = false;
    gameOver = false;
    score = 0;
}

/**
 * 게임 퇴장
 */
void GameManager::onExitGame() {
    
    onGamePause();
    
    view = nullptr;
    
    listeners.clear();
}

Node* GameManager::getView() {
    return instance->view;
}

#pragma mark- GameListener

/**
 * 게임 시작
 */
void GameManager::onGameStart() {
    
    gamePaused = false;
    updateLocked = false;
    
    for( auto listener : listeners ) {
        listener->onGameStart();
    }
}

/**
 * 게임 재시작
 */
void GameManager::onGameRestart() {
    
    gamePaused = false;
    gameOver = false;
    updateLocked = false;
    
    for( auto listener : listeners ) {
        listener->onGameRestart();
    }
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



