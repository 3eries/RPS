//
//  Man.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#include "Man.hpp"

#include "SceneManager.h"
#include "../GameDefine.h"

USING_NS_CC;
using namespace std;

// idle 애니메이션
static const vector<string> ANIM_IDLE_FILES({
    DIR_IMG_GAME + "RSP_avatar_idle1.png",
    DIR_IMG_GAME + "RSP_avatar_idle2.png",
});

static const float ANIM_IDLE_DELAY_PER_UNIT = 0.5f;

// attack 애니메이션
static const vector<string> ANIM_ATTACK_FILES({
    DIR_IMG_GAME + "RSP_avatar_attack1.png",
    DIR_IMG_GAME + "RSP_avatar_attack2.png",
    // DIR_IMG_GAME + "RSP_avatar_idle1.png",
});

static const float ANIM_ATTACK_PER_UNIT = 0.0666f;

// 스케줄러
static const string SCHEDULER_FEVER_GAGE_RESET = "SCHEDULER_FEVER_GAGE_RESET";   // 피버 포인트 게이지 초기화

Man::Man() :
gameMgr(GameManager::getInstance()) {
}

Man::~Man() {
}

bool Man::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    initImage();
    initFeverGage();
    
    setAnchorPoint(ANCHOR_MB);
    setManPosition(Position::LEFT);
    
    gameMgr->addListener(this);
    
    return true;
}

void Man::onExit() {
    
    clearRemoveNodes();
    gameMgr->removeListener(this);
    
    Node::onExit();
}

/**
 * 캐릭터 리셋
 */
void Man::reset() {
    
    clearRemoveNodes();
    
    setFeverPoint(0);
    setFeverGageVisible(false);
    
    setManAnimation(AnimationType::IDLE);
    setManPosition(Position::LEFT);
}

/**
 * 삭제할 노드 정리
 */
void Man::clearRemoveNodes() {
    
    for( auto node : needRemoveNodes ) {
        node->removeFromParent();
    }
    
    needRemoveNodes.clear();
}

/**
 * 게임 시작
 */
void Man::onGameStart() {
    
    reset();
}

/**
 * 게임 재시작
 */
void Man::onGameRestart() {
    
    setVisible(true);
    clearRemoveNodes();
}

void Man::onGamePause() {
}

void Man::onGameResume() {
}

/**
 * 게임 오버 전
 */
void Man::onPreGameOver() {
    
    // 사망 애니메이션
    runDieAnimation();
    
    unscheduleAllCallbacks();
}

/**
 * 이어하기
 */
void Man::onContinue() {
    
    setVisible(true);
    reset();
}

void Man::onGameOver() {
}

/**
 * 타이머 시작
 */
void Man::onStartTimer() {
    
    setFeverGageVisible(true);
}

/**
 * 게임 모드 전환
 */
void Man::onGameModeChanged(GameMode mode) {
    
    switch( mode ) {
        case GameMode::NORMAL: {
            resetFeverPoint(false);
        } break;
            
        case GameMode::FEVER: {
            unschedule(SCHEDULER_FEVER_GAGE_RESET);
            setFeverGageVisible(false);
        } break;
    }
}

/**
 * 마지막 피버 블럭 히트
 */
void Man::onLastFeverBlockHit() {
    
    setFeverGageVisible(true);
}

/**
 * 애니메이션 설정
 */
void Man::setManAnimation(AnimationType animType, bool runAnimation) {
    
    img->stopAnimation();
    
    switch( animType ) {
        case AnimationType::IDLE: {
            auto anim = SBNodeUtils::createAnimation(ANIM_IDLE_FILES, ANIM_IDLE_DELAY_PER_UNIT);
            // anim->setRestoreOriginalFrame(true);
            
            img->setAnimation(anim);
            
        } break;
            
        case AnimationType::ATTACK: {
            auto anim = SBNodeUtils::createAnimation(ANIM_ATTACK_FILES, ANIM_ATTACK_PER_UNIT);
            anim->setRestoreOriginalFrame(true);
            
            img->setAnimation(anim, 1);
            
        } break;
            
        default:
            CCASSERT(false, "Man::setManAnimation error: invalid animation.");
            break;
    }
    
    if( runAnimation ) {
        img->runAnimation();
    }
}

/**
 * 좌표 설정
 */
void Man::setManPosition(Position pos) {
    
    manPosition = pos;
    
    switch( pos ) {
        case Position::LEFT: {
            img->setFlippedX(false);
            setPosition(MAN_POS_LEFT);
            
            feverGage.bg->setPosition(Vec2TC(getContentSize(), -24, 22));
            
        } break;
            
        case Position::RIGHT: {
            img->setFlippedX(true);
            setPosition(MAN_POS_RIGHT);
            
            feverGage.bg->setPosition(Vec2TC(getContentSize(), 24, 22));
            
        } break;
            
        default:
            CCASSERT(false, "Man::setManPosition error: invalid position.");
            break;
    }
}

/**
 * 피버 포인트 설정
 */
void Man::setFeverPoint(float point, bool isUpdateGage) {
 
    const int maxPoint = gameMgr->getConfig()->getFeverInfo().maxPoint;
    
    point = MAX(0, point);
    point = MIN(maxPoint, point);
    feverGage.point = point;
    
    if( isUpdateGage ) {
        updateFeverGage();
    }
    
    // 피버 포인트 충족 시 피버 발동
    if( point == maxPoint ) {
        gameMgr->onFeverMode();
    }
}

void Man::addFeverPoint(float point, bool isUpdateGage) {
 
    setFeverPoint(feverGage.point + point, isUpdateGage);
}

/**
 * 피버 포인트 초기화
 */
void Man::resetFeverPoint(bool isRunAction) {
    
    setFeverPoint(0, !isRunAction);
    
    // 게이지 초기화 연출
    if( isRunAction ) {
        auto scale = ScaleTo::create(0.07f, 0, 1);
        feverGage.gage->runAction(scale);
    }
}

/**
 * 피버 게이지 업데이트
 */
void Man::updateFeverGage() {
    
    feverGage.gage->stopAllActions();
    
    int maxPoint = gameMgr->getConfig()->getFeverInfo().maxPoint;
    float ratio = (float)feverGage.point / maxPoint;
    bool isIncrease = (ratio > feverGage.gage->getScaleX());
    
    feverGage.gage->setScaleX(ratio);
    
    // 게이지가 증가한 경우, 흰색 이펙트
    if( isIncrease ) {
        feverGage.whiteGage->setVisible(true);
        feverGage.whiteGage->setScaleX(ratio);
        
        auto delay = DelayTime::create(0.04f);
        auto hide = Hide::create();
        feverGage.whiteGage->runAction(Sequence::create(delay, hide, nullptr));
    }
}

/**
 * 승패 처리
 */
void Man::showdown(RSPResult result, RSPType myHand, RSPType oppHand) {
    
    CCLOG("Man::showdown: %d", (int)result);
    
    switch( result ) {
        case RSPResult::WIN:    this->resultWin(myHand, oppHand);      break;
        case RSPResult::LOSE:   this->resultLose(myHand, oppHand);     break;
        case RSPResult::DRAW:   this->resultDraw(myHand, oppHand);     break;
        default: break;
    }
}

/**
 * 락앤롤!!!
 */
void Man::rockNroll(Position pos) {
    
    SBAudioEngine::play2d(SOUND_PUNCH);
    
    runAttackAnimation();
    
    setManPosition(pos);
}

/**
 * 승리!
 */
void Man::resultWin(RSPType myHand, RSPType oppHand) {
    
    SBAudioEngine::play2d(SOUND_PUNCH);
    
    runAttackAnimation();
    
    switch( myHand ) {
        case RSPType::ROCK:     setManPosition(Position::LEFT);     break;
        case RSPType::PAPER:    setManPosition(Position::RIGHT);    break;
        case RSPType::SCISSORS: {
            
        } break;
            
        default:
            CCASSERT(false, "Man::hit error: invalid block type.");
            break;
    }
    
    // fever point
//    if( !gameMgr->isFeverMode() ) {
    auto feverInfo = gameMgr->getConfig()->getFeverInfo();
    addFeverPoint(feverInfo.points[myHand]);
    
    // 피버 게이지 초기화 스케줄러 실행
    unschedule(SCHEDULER_FEVER_GAGE_RESET);
    
    if( gameMgr->getGameMode() == GameMode::NORMAL ) {
        scheduleOnce([=](float dt) {
            this->resetFeverPoint();
        }, feverInfo.gageDuration, SCHEDULER_FEVER_GAGE_RESET);
    }
}

/**
 * 패배
 */
void Man::resultLose(RSPType myHand, RSPType oppHand) {
}

/**
 * 무승부
 */
void Man::resultDraw(RSPType myHand, RSPType oppHand) {
    
//if( !gameMgr->isFeverMode() ) {
    resetFeverPoint();
    
    // blink
    /*
    float delay = gameMgr->getConfig()->getTimeInfo().drawDelay;
    
    auto blink = Blink::create(delay, 2);
    img->runAction(blink);
     */
    
    runAttackAnimation();
}

/**
 * 공격 애니메이션 재생
 */
void Man::runAttackAnimation() {
    
    // switch attack animation
    setManAnimation(AnimationType::ATTACK, false);
    
    img->runAnimation([=](Node*) {
        // restore animation
        setManAnimation(AnimationType::IDLE);
    });
}

/**
 * 사망 애니메이션 재생
 */
void Man::runDieAnimation() {
    
    // 벼락 효과음
    SBAudioEngine::play2d(SOUND_THUNDER);
    
    // die 애니메이션 재생
    string animName = "";
    
    switch( manPosition ) {
        case Position::LEFT:    animName = ANIM_NAME_DIE_LEFT;      break;
        case Position::RIGHT:   animName = ANIM_NAME_DIE_RIGHT;     break;
        default:
            CCASSERT(false, "Man::resultLose error: invalid man position");
            break;
    }
    
    // 사망 애니메이션
    auto anim = SBSpineHelper::runAnimation(nullptr, ANIM_DIE, animName);
    SceneManager::getScene()->addChild(anim, SBZOrder::BOTTOM);
    
    needRemoveNodes.pushBack(anim);
    
    anim->setEventListener([=](spTrackEntry *track, spEvent *event) {
        
        string eventName = event->data->name;
        
        if( eventName == ANIM_EVENT_DIE ) {
            this->setVisible(false);
        }
    });
}

bool Man::isPositionLeft() {
    
    return manPosition == Man::Position::LEFT;
}

void Man::setFeverGageVisible(bool visible) {
    
    feverGage.bg->setVisible(visible);
}

/**
 * 이미지 초기화
 */
void Man::initImage() {
    
    auto anim = SBNodeUtils::createAnimation(ANIM_IDLE_FILES, ANIM_IDLE_DELAY_PER_UNIT);
//    anim->setRestoreOriginalFrame(true);
    
    img = SBAnimationSprite::create(anim);
    img->setAnchorPoint(ANCHOR_M);
    img->setPosition(Vec2MC(img->getContentSize(), 0, 0));
    addChild(img);
    
    setContentSize(img->getContentSize());
    
    img->runAnimation();
}

/**
 * 피버 게이지 초기화
 */
void Man::initFeverGage() {
 
    feverGage.bg = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_bg.png");
    feverGage.bg->setVisible(SceneManager::getSceneType() == SceneType::GAME);
    feverGage.bg->setAnchorPoint(ANCHOR_MB);
    addChild(feverGage.bg);

    auto bgSize = feverGage.bg->getContentSize();

    // gage
    feverGage.gage = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_green.png");
    feverGage.gage->setAnchorPoint(ANCHOR_ML);
    feverGage.gage->setPosition(Vec2ML(bgSize,
                                       (bgSize.width-feverGage.gage->getContentSize().width)*0.5f, 0));
    feverGage.gage->setScaleX(0);
    feverGage.bg->addChild(feverGage.gage);
    
    // white gage
    feverGage.whiteGage = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_white.png");
    feverGage.whiteGage->setVisible(false);
    feverGage.whiteGage->setAnchorPoint(feverGage.gage->getAnchorPoint());
    feverGage.whiteGage->setPosition(feverGage.gage->getPosition());
    feverGage.bg->addChild(feverGage.whiteGage);
    
    /*
    auto bar = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_green.png");
     
    feverGage.gage = ProgressTimer::create(bar);
    feverGage.gage->setType(ProgressTimer::Type::BAR);
    feverGage.gage->setMidpoint(ANCHOR_ML);
    feverGage.gage->setBarChangeRate(Vec2(1, 0));
    feverGage.gage->setAnchorPoint(ANCHOR_M);
    feverGage.gage->setPosition(Vec2MC(feverGage.bg->getContentSize(), 0, 0));
    feverGage.bg->addChild(feverGage.gage);
    */
}
