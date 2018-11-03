//
//  GameManager.cpp
//
//  Created by seongmin hwang on 2018. 2. 23..
//

#include "GameManager.hpp"

#include "UserDefaultKey.h"
#include "RankingManager.hpp"

#include "GameDefine.h"
#include "GameView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static const string SCHEDULER_FEVER_END       = "GM_SCHEDULER_FEVER_END";           // 피버 모드 종료
static const string SCHEDULER_FEVER_END_ALERT = "GM_SCHEDULER_FEVER_END_ALERT";     // 피버 모드 종료 알림

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
    
    // log
    {
        string log;
        log += "\t" + STR_FORMAT("play count: %d", getPlayCount()) + "\n";
        
        CCLOG("GameManager::init\n{\n%s}", log.c_str());
    }
}

void GameManager::reset() {
    
    updateLocked = false;
    gamePaused = false;
    preGameOver = false;
    gameOver = false;
    gameMode = GameMode::NORMAL;
    drawCount = 0;
    feverModeCount = 0;
    continueCount = 0;
    levelInfo = GameConfiguration::getInstance()->getLevelInfo(1);
    score = 0;
    ranking = INVALID_RANKING;
    unlockedCharacters.clear();
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
    reset();
    
    listeners.clear();
}

/**
 * 스코어 설정
 */
void GameManager::setScore(int score) {
    
    score = MIN(config->getMaxScore(), score);
    this->score = score;
    
    // 스코어에 해당하는 레벨 설정
    auto saveLevelInfo = this->levelInfo;
    auto infos = GameConfiguration::getInstance()->getLevelInfos();
    
    for( auto info : infos ) {
        if( this->score >= info.beginRange ) {
            this->levelInfo = info;
        }
    }
    
    // 레벨 변경 체크
    bool isLevelChanged = (saveLevelInfo.level != this->levelInfo.level);
    
    if( isLevelChanged ) {
        onLevelChanged();
    }
    
    // CCLOG("GameManager::setScore: %d level: %d", this->score, this->levelInfo.level);
}

void GameManager::addScore(int score) {
    
    setScore(this->score + score);
}

bool GameManager::isContinuable() {
    
    return getScore() >= config->getContinueCondition() && getContinueCount() == 0;
}

bool GameManager::isNewRecord() {
    
    return ranking != INVALID_RANKING;
}

int GameManager::getPlayCount() {
    
    return UserDefault::getInstance()->getIntegerForKey(UserDefaultKey::PLAY_COUNT, 0);
}

GameView* GameManager::getView() {
    
    return instance->view;
}

#pragma mark- GameListener

/**
 * 게임 시작
 */
void GameManager::onGameStart() {
    
    firebase::Analytics::logEvent(FA_EVENT_GAME_PLAY);
    
    UserDefault::getInstance()->setIntegerForKey(UserDefaultKey::PLAY_COUNT, getPlayCount()+1);
    UserDefault::getInstance()->flush();
    
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
//    onGameResume(true);
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
void GameManager::onGameResume(bool force) {
    
    if( !force ) {
        if( !gamePaused ) {
            return;
        }
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
    
    preGameOver = true;
    
//    onGamePause();
    view->unschedule(SCHEDULER_FEVER_END);
    view->unschedule(SCHEDULER_FEVER_END_ALERT);
    
    for( auto listener : listeners ) {
        listener->onPreGameOver();
    }
}

/**
 * 이어하기
 */
void GameManager::onContinue() {
   
    ++continueCount;
    preGameOver = false;
    
    // 피버 모드 였다면, 노멀 모드로 전환
    if( gameMode == GameMode::FEVER ) {
        onNormalMode();
    }
    
    for( auto listener : listeners ) {
        listener->onContinue();
    }
    
//    onGameResume();
}

static void logEventGameResult(int score) {
    
    if( score < 1 ) {
        return;
    }
    
    // 스코어 범위 획득 함수
    auto getScoreRange = [](int score) -> string {
        
        int begin = 1;
        int end   = 100;

        while( true ) {
            if( score >= begin && score <= end ) {
                break;
            }
            
            int gap = (end < 800) ? 100 : 50;
            
            begin = (end + 1);
            end += gap;
        }
        
        return STR_FORMAT("%d ~ %d", begin, end);
    };
    
    // firebase 이벤트 기록
    auto gameMgr = GameManager::getInstance();
    
    const int feverModeCount = gameMgr->getFeverModeCount();
    
    firebase::Analytics::EventParams params;
    params[FA_EVENT_PARAM_SCORE] = Value(score);
    params[FA_EVENT_PARAM_SCORE_RANGE] = Value(getScoreRange(score));
    params[FA_EVENT_PARAM_FEVER] = Value(feverModeCount);
    params[FA_EVENT_PARAM_FIRST_FEVER] = Value(feverModeCount > 0 ? 1 : 0);
    params[FA_EVENT_PARAM_DRAW] = Value(gameMgr->getDrawCount());
    
    firebase::Analytics::logEvent(FA_EVENT_GAME_RESULT, params);
}

/**
 * 게임 오버
 */
void GameManager::onGameOver() {
    
    if( gameOver ) {
        return;
    }
    
    gameOver = true;
    preGameOver = false;
    updateLocked = true;
    
    // 캐릭터 리스너 초기화
    auto charMgr = CharacterManager::getInstance();
    
    CharacterListener characterListener;
    characterListener.onCharacterUnlocked = [=](Characters characters) {
        SBCollection::addAll(unlockedCharacters, characters);
    };
    
    if( score > 0 ) {
        logEventGameResult(score);
    
        // 패키지 DB 업데이트
        charMgr->submit(characterListener, PackageDB::Field::BEST_SCORE, score);
        charMgr->submit(characterListener, PackageDB::Field::TOTAL_SCORE, score);
        
        // 순위 선정
        ranking = RankingManager::getNewRanking(score);
        
        // 리더보드 등록
        superbomb::PluginPlay::submitScore(LEADER_BOARD_HIGH_SCORE, score);
    }
    
    // 패키지 DB 업데이트
    charMgr->submit(characterListener, PackageDB::Field::GAME_PLAY);
    charMgr->submit(characterListener, PackageDB::Field::GAME_OVER);
    charMgr->submit(characterListener, PackageDB::Field::FEVER, feverModeCount);
    charMgr->commitAll();
    
    // 리스너 실행
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
 * 타이머 시작
 */
void GameManager::onStartTimer() {

    for( auto listener : listeners ) {
        listener->onStartTimer();
    }
}

/**
 * 레벨 변경
 */
void GameManager::onLevelChanged() {
    
    for( auto listener : listeners ) {
        listener->onLevelChanged(levelInfo);
    }
}

/**
 * 블럭 비김
 */
void GameManager::onDrawBlock() {
    
    ++drawCount;
    
    for( auto listener : listeners ) {
        listener->onDrawBlock();
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
 * 피버 모드로 전환
 */
void GameManager::onFeverMode() {
    
    ++feverModeCount;
    onGameModeChanged(GameMode::FEVER);
    
    // 피버 모드 종료 스케줄러
    const float DURATION = config->getFeverInfo().duration;
    const float END_ALERT_DELAY = DURATION - FEVER_END_ALERT_TIME;
    
    view->scheduleOnce([=](float dt) {
        // 노멀 모드로 복구
        this->onNormalMode();

    }, DURATION, SCHEDULER_FEVER_END);
    
    // 피버 모드 종료 알림 스케줄러
    view->scheduleOnce([=](float dt) {
        
        this->onPreFeverModeEnd();
        
    }, END_ALERT_DELAY, SCHEDULER_FEVER_END_ALERT);
}

/**
 * 피버 모드 종료 전
 */
void GameManager::onPreFeverModeEnd() {
    
    for( auto listener : listeners ) {
        listener->onPreFeverModeEnd();
    }
}

/**
 * 마지막 피버 블럭 히트
 */
void GameManager::onLastFeverBlockHit() {
    
    for( auto listener : listeners ) {
        listener->onLastFeverBlockHit();
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



