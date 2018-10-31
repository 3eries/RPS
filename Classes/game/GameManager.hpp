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

#include "Define.h"
#include "GameConfiguration.hpp"

#include "CharacterManager.hpp"

class GameScene;
class GameView;

enum class GameMode {
    NORMAL,
    FEVER, // rock n roll
};

#pragma mark- GameListener

class GameListener {
public:
    virtual void onGameStart() {}                      // 게임 시작
    virtual void onGameRestart() {}                    // 게임 재시작
    virtual void onGamePause() {}                      // 게임 일시정지
    virtual void onGameResume() {}                     // 게임 재게
    virtual void onPreGameOver() {}                    // 게임 오버 전, 이어하기 상태 처리
    virtual void onContinue() {}                       // 이어하기
    virtual void onGameOver() {}                       // 게임 오버
    virtual void onGameEnd() {}                        // 게임 종료
    
    virtual void onStartTimer() {}                     // 타이머 시작
    virtual void onLevelChanged(LevelInfo level) {}    // 레벨 변경
    virtual void onDrawBlock() {}                      // 블럭 비김
    virtual void onGameModeChanged(GameMode mode) {}   // 게임 모드 전환
    virtual void onPreFeverModeEnd() {}                // 피버 모드 종료 전
    virtual void onLastFeverBlockHit() {}              // 마지막 피버 블럭 히트
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
    
    void setScore(int score);
    void addScore(int score);
    
    bool isContinuable();
    bool isNewRecord();
    
    static int getPlayCount();
    
    static GameView* getView();
    
private:
    void reset();
    
private:
    CC_SYNTHESIZE_READONLY(GameConfiguration*, config, Config);
    GameView *view;   // 게임 View
    
    SB_SYNTHESIZE_BOOL(updateLocked, UpdateLocked);               // update 스케줄러 잠금 여부
    SB_SYNTHESIZE_BOOL(gamePaused, GamePaused);                   // 게임 일시정지 여부
    SB_SYNTHESIZE_BOOL(preGameOver, PreGameOver);                 // 게임 오버 직전 상태 여부
    SB_SYNTHESIZE_BOOL(gameOver, GameOver);                       // 게임 오버 여부
    CC_SYNTHESIZE_READONLY(GameMode, gameMode, GameMode);         // 게임 모드
    
    CC_SYNTHESIZE_READONLY(int, drawCount, DrawCount);            // 비긴 총 횟수
    CC_SYNTHESIZE_READONLY(int, feverModeCount, FeverModeCount);  // 피버 모드한 횟수
    CC_SYNTHESIZE_READONLY(int, continueCount, ContinueCount);    // 이어하기한 횟수
    CC_SYNTHESIZE(LevelInfo, levelInfo, LevelInfo);               // 현재 레벨 정보
    CC_SYNTHESIZE_READONLY(int, score, Score);                    // 현재 스코어
    CC_SYNTHESIZE_READONLY(int, ranking, Ranking);                // 최종 스코어에 해당하는 순위
    
    // 획득 캐릭터
    CC_SYNTHESIZE_READONLY(Characters,
                           unlockedCharacters, UnlockedCharacters);
    
// GameListener
public:
    void onGameStart();
    void onGameRestart();
    void onGamePause();
    void onGameResume(bool force = false);
    void onPreGameOver();
    void onContinue();
    void onGameOver();
    void onGameEnd();
    
    void onStartTimer();
    void onLevelChanged();
    void onDrawBlock();
    
    void onGameModeChanged(GameMode mode);
    void onNormalMode();
    void onFeverMode();
    
    void onPreFeverModeEnd();
    void onLastFeverBlockHit();
    
    void addListener(GameListener *listener);
    void removeListener(GameListener *listener);
    
private:
    std::vector<GameListener*> listeners;
};

#endif /* GameManager_hpp */
