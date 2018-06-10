//
//  Man.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 22..
//

#include "Man.hpp"

#include "../GameDefine.h"

USING_NS_CC;
using namespace std;

static const vector<string> ANIM_IDLE_FILES({
    DIR_IMG_GAME + "RSP_avatar_idle1.png",
    DIR_IMG_GAME + "RSP_avatar_idle2.png",
});

static const float ANIM_IDLE_DELAY_PER_UNIT = 0.5f;

static const vector<string> ANIM_ATTACK_FILES({
    DIR_IMG_GAME + "RSP_avatar_attack1.png",
    DIR_IMG_GAME + "RSP_avatar_attack2.png",
    // DIR_IMG_GAME + "RSP_avatar_idle1.png",
});

static const float ANIM_ATTACK_PER_UNIT = 0.0666f;

static const string SCHEDULER_FEVER = "SCHEDULER_FEVER";
static const string SCHEDULER_FEVER_END_ALERT = "SCHEDULER_FEVER_END_ALERT";
static const string SCHEDULER_FEVER_GAGE_RESET = "SCHEDULER_FEVER_GAGE_RESET";

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
    ViewManager::getInstance()->addListener(this);
    
    return true;
}

void Man::onExit() {
    
    ViewManager::getInstance()->removeListener(this);
    gameMgr->removeListener(this);
    
    Node::onExit();
}

/**
 * 뷰 타입 변경
 */
void Man::onViewChanged(ViewType viewType) {
    
    switch( viewType ) {
        case ViewType::MAIN: {
            feverGage.bg->setVisible(false);
        } break;
            
        case ViewType::GAME: {
            feverGage.bg->setVisible(true);
        } break;
            
        default:
            break;
    }
}

void Man::onGameStart() {
    
    lastWinHand = RSPType::NONE;
    lastShowdownTime = 0;
    
    setFeverPoint(0);
    
    setManAnimation(AnimationType::IDLE);
    setManPosition(Position::LEFT);
    
    scheduleUpdate();
}

void Man::onGameRestart() {
}

void Man::onGameOver() {
    
    unscheduleAllCallbacks();
    unscheduleUpdate();
}

void Man::onGamePause() {
    
    pause();
}

void Man::onGameResume() {
    
    resume();
}

/**
 * 게임 모드 전환
 */
void Man::onGameModeChanged(GameMode mode) {
    
    switch( mode ) {
        case GameMode::NORMAL: {
            resetFeverPoint();
        } break;
            
        case GameMode::FEVER: {
            unschedule(SCHEDULER_FEVER_GAGE_RESET);
        } break;
    }
}

void Man::update(float dt) {
    
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
            
            feverGage.bg->setPosition(Vec2TC(getContentSize(), -24, 12));
            
        } break;
            
        case Position::RIGHT: {
            img->setFlippedX(true);
            setPosition(MAN_POS_RIGHT);
            
            feverGage.bg->setPosition(Vec2TC(getContentSize(), 24, 12));
            
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
    
    // 피버 모드 발동 체크
    if( point == maxPoint ) {
        runFeverMode();
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
    
    const int maxPoint = gameMgr->getConfig()->getFeverInfo().maxPoint;
    
//    float per = ((float)feverGage.point / maxPoint) * 100;
//    feverGage.gage->setPercentage(per);
    feverGage.gage->setScaleX((float)feverGage.point / maxPoint);
}

/**
 * 피버 모드 발동
 */
void Man::runFeverMode() {
    
    gameMgr->onFeverMode();
    
    // 피버 모드 종료 스케줄러
    float duration = gameMgr->getConfig()->getFeverInfo().duration;
    
    scheduleOnce([=](float dt) {
        // 노멀 모드로 복구
        this->resetFeverPoint();
        gameMgr->onNormalMode();
        
    }, duration, SCHEDULER_FEVER);
    
    // 피버 모드 종료 알림 스케줄러
    scheduleOnce([=](float dt) {
        
        gameMgr->onPreFeverModeEnd();
        
    }, duration - FEVER_END_ALERT_TIME, SCHEDULER_FEVER_END_ALERT);
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
    
    // switch attack animation
    setManAnimation(AnimationType::ATTACK, false);
    
    img->runAnimation([=](Node*) {
        // restore animation
        setManAnimation(AnimationType::IDLE);
    });
    
    setManPosition(pos);
}

/**
 * 승리!
 */
void Man::resultWin(RSPType myHand, RSPType oppHand) {
    
    lastWinHand = myHand;
    
    SBAudioEngine::play2d(SOUND_PUNCH);
    
    // switch attack animation
    setManAnimation(AnimationType::ATTACK, false);
    
    img->runAnimation([=](Node*) {
        // restore animation
        setManAnimation(AnimationType::IDLE);
    });
    
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
    
    float delay = gameMgr->getConfig()->getTimeInfo().drawDelay;
    
    auto blink = Blink::create(delay, 2);
    img->runAction(blink);
    
//if( !gameMgr->isFeverMode() ) {
    resetFeverPoint();
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
    feverGage.bg->setAnchorPoint(ANCHOR_MB);
    feverGage.bg->setPosition(Vec2TC(getContentSize(), -24, 12));
    addChild(feverGage.bg);

    auto bgSize = feverGage.bg->getContentSize();
    
    feverGage.gage = Sprite::create(DIR_IMG_GAME + "RSP_gage_fever_green.png");
    feverGage.gage->setAnchorPoint(ANCHOR_ML);
    feverGage.gage->setPosition(Vec2ML(bgSize,
                                       (bgSize.width-feverGage.gage->getContentSize().width)*0.5f, 0));
    feverGage.gage->setScaleX(0);
    feverGage.bg->addChild(feverGage.gage);
    
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
