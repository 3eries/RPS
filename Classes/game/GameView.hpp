//
//  GameView.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameView_hpp
#define GameView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RSP.h"
#include "GameConfiguration.hpp"
#include "GameManager.hpp"

class RSPBlock;
class RSPBlockLayer;
class RSPButtonLayer;
class Man;
class TimeBar;
class DarkCloud;

class GameView : public cocos2d::Node, public GameListener {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    enum Tag {
        BG                 = 1,
        CLOUD,
        
        FEVER_MODE_BG,
        FEVER_MODE_FIRE,
        
        LABEL_LEVEL,       // 레벨
        LABEL_SCORE,       // 스코어
    };
    
    enum class ZOrder {
        BG = -2,
        CLOUD = -1,
        BLOCK = 0,
    };
    
private:
    GameView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initBlock();
    void initRSPButton();
    void initMan();
    void initTimeBar();
    void initLabels();
    
    void reset();
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGamePause() override;
    void onGameResume() override;
    void onPreGameOver() override;
    void onContinue() override;
    void onGameOver() override;
    void onStartTimer() override;
    void onLevelChanged(LevelInfo level) override; 
    void onGameModeChanged(GameMode mode) override;
    void onPreFeverModeEnd() override;
    
private:
    void updateScore();
    void updateButtonMode();
    
    void onClickNormalButton(RSPType type);
    void onClickFeverButton(int i);
    
    void hitBlock(RSPBlock *block, RSPType btnType);
    void misBlock(RSPBlock *block);
    void drawBlock(RSPBlock *block);
    
private:
    void showLevelLabel();
    
private:
    GameManager *gameMgr;
    
    // 블럭 히트 횟수
    int hitCount;
    
    // 블럭
    RSPBlockLayer *blockLayer;
    
    // 가위바위보 버튼 레이어
    RSPButtonLayer *buttonLayer;
    
    // 캐릭터
    Man *man;
    
    // 시간 제한바
    TimeBar *timeBar;
    
    // 먹구름
    DarkCloud *darkCloud;
};

#endif /* GameView_hpp */
