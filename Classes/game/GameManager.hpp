//
//  GameManager.hpp
//
//  Created by seongmin hwang on 2018. 2. 23..
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "RSP.h"

class GameScene;
class GameView;

#pragma mark- GameListener

class GameListener {
public:
    virtual void onGameStart() {}       // 게임 시작
    virtual void onGameRestart() {}     // 게임 재시작
    virtual void onGamePause() {}       // 게임 일시정지
    virtual void onGameResume() {}      // 게임 재게
    virtual void onPreGameOver() {}     // 게임 오버 전, Continue 상태 처리
    virtual void onGameOver() {}        // 게임 오버
    virtual void onGameEnd() {}         // 게임 종료, 모든 스테이지 완료
};

#pragma mark- GameManager

class GameManager {
public:
    static GameManager* getInstance();
    static void destroyInstance();
    ~GameManager();
    
private:
    GameManager();
    
public:
    void init();
    
    void onEnterGame(GameView *view);
    void onExitGame();
    
// getter
public:
    static cocos2d::Node* getScene();
    static cocos2d::Node* getView();
    
private:
    GameView *view;   // 게임 View
    
    SB_SYNTHESIZE_BOOL(updateLocked, UpdateLocked);     // update 스케줄러 잠금 여부
    SB_SYNTHESIZE_BOOL(gamePaused, GamePaused);         // 게임 일시정지 여부
    SB_SYNTHESIZE_BOOL(gameOver, GameOver);             // 게임 오버 여부
    
    CC_SYNTHESIZE(size_t, score, Score);                // 현재 스코어
    
// GameListener
public:
    void onGameStart();
    void onGameRestart();
    void onGamePause();
    void onGameResume();
    void onPreGameOver();
    void onGameOver();
    void onGameEnd();
    
    void addListener(GameListener *listener);
    void removeListener(GameListener *listener);
    
private:
    std::vector<GameListener*> listeners;
};

#endif /* GameManager_hpp */
