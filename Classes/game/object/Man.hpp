//
//  Man.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#ifndef Man_hpp
#define Man_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Define.h"

#include "../GameManager.hpp"
#include "CharacterManager.hpp"

class Man : public cocos2d::Node, public GameListener {
public:
    static const int ACTION_TAG_DRAW_MOVE = 1000;
    
    enum class Position {
        LEFT,
        RIGHT,
    };
    
    enum class AnimationType {
        IDLE,
        ATTACK,
    };
    
public:
    static Man* create(Character character);
    static Man* create();
    ~Man();
    
private:
    Man(Character character);
    
    bool init() override;
    void onEnter() override;
    void onExit() override;
    
    void initImage();
    void initFeverGage();
    
    cocos2d::Animation* createAnimation(AnimationType animType);
    
public:
    void setCharacter(Character character, bool isReset = true);
    
    void reset();
    void clearRemoveNodes();
    
    void setAnimation(AnimationType animType, bool runAnimation = true,
                      SBCallback onCompleted = nullptr);
    void setManPosition(Position pos);
    
    void setFeverPoint(float point, bool isUpdateGage = true);
    void addFeverPoint(float point, bool isUpdateGage = true);
    void resetFeverPoint(bool isRunAction = true);
    void updateFeverGage();
    float   getFeverGageRatio();
    
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
    void onGameModeChanged(GameMode mode) override;
    void onLastFeverBlockHit() override;
    
public:
    void showdown(RSPResult result, RSPType myHand, RSPType oppHand);
    void rockNroll(Position pos);
    
    void resultWin(RSPType myHand, RSPType oppHand);
    void resultLose(RSPType myHand, RSPType oppHand);
    void resultDraw(RSPType myHand, RSPType oppHand);
    
    void playAttackSound();
    
    void runAttackAnimation();
    void runDieAnimation();
    
    void setFeverGageVisible(bool visible);
    
    bool isPositionLeft();
    
private:
    GameManager *gameMgr;
    
    // 캐릭터
    Character character;
    SBAnimationSprite *img;
    
    CC_SYNTHESIZE_READONLY(Position, manPosition, ManPosition);
    
    // 피버 게이지
    struct FeverGage {
        float point;
        cocos2d::Node *bg;
        cocos2d::Sprite *gage;
        cocos2d::Sprite *whiteGage;
        
        FeverGage() {
            point = 0;
        }
    };
    
    FeverGage feverGage;

    // 삭제 예정 노드 리스트 (리셋 시)
    cocos2d::Vector<cocos2d::Node*> needRemoveNodes;
    
    int attackSoundPlayCount;   // 공격 효과음 재생 횟수
    int voiceSoundIndex;        // 보이스 효과음 인덱스
};

#endif /* Man_hpp */
