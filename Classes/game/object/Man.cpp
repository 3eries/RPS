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

// 스케줄러
static const string SCHEDULER_FEVER_GAGE_RESET = "SCHEDULER_FEVER_GAGE_RESET";   // 피버 포인트 게이지 초기화

Man* Man::create(Character character) {
    
    auto man = new Man(character);
    
    if( man && man->init() ) {
        man->autorelease();
        return man;
    }
    
    delete man;
    return nullptr;
}

Man* Man::create() {
    
    return create(CharacterManager::getInstance()->getSelectedCharacter());
}

Man::Man(Character character) :
character(character),
gameMgr(GameManager::getInstance()),
attackSoundPlayCount(0),
voiceSoundIndex(0) {
}

Man::~Man() {
    CharacterManager::getInstance()->removeListener(this);
}

bool Man::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    initImage();
    initFeverGage();
    
    setAnchorPoint(ANCHOR_M);
    setManPosition(Position::LEFT);
    setFeverGageVisible(false);
    
    gameMgr->addListener(this);
    
    return true;
}

void Man::onEnter() {
    
    Node::onEnter();
    
    // 캐릭터 리스너 초기화
    auto listener = CharacterListener::create();
    listener->setTarget(this);
    listener->onCharacterSelected = [=](const Character &character) {
        this->setCharacter(character, !SceneManager::isGameScene());
    };
    
    CharacterManager::getInstance()->addListener(listener);
}

void Man::onExit() {
    
    clearRemoveNodes();
    
    CharacterManager::getInstance()->removeListener(this);
    gameMgr->removeListener(this);
    
    Node::onExit();
}

/**
 * 캐릭터 설정
 */
void Man::setCharacter(Character character, bool isReset) {
    
    this->character = character;
    
    if( isReset ) {
        reset();
    }
}

/**
 * 캐릭터 리셋
 */
void Man::reset() {
    
    clearRemoveNodes();
    
    setFeverPoint(0);
    setFeverGageVisible(false);
    
    setAnimation(AnimationType::IDLE);
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
    
    // 1UP 효과
    auto manBox = SBNodeUtils::getBoundingBoxInWorld(this);
    
    auto label = Label::createWithTTF("1UP", FONT_RETRO, 45);
    label->setAnchorPoint(ANCHOR_M);
    label->setPosition(Vec2(manBox.getMidX() - 15, manBox.getMaxY() + 30));
    label->setColor(Color3B(119, 255, 0));
    label->enableOutline(Color4B::BLACK, 4);
    SceneManager::getScene()->addChild(label, SBZOrder::BOTTOM);
    
    scheduleOnce([=](float dt) {
        
        SBAudioEngine::playEffect(SOUND_LIFE_UP);
        
        auto move = MoveBy::create(0.8f, Vec2(0, 70));
        auto remove = RemoveSelf::create();
        label->runAction(Sequence::create(move, remove, nullptr));
        
    }, 0.2f, "CONTINUE_EFFECT_DELAY");
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
void Man::setAnimation(AnimationType animType, bool runAnimation, SBCallback onCompleted) {
    
    img->stopAnimation();
    
    int loops = (animType == AnimationType::ATTACK) ? 1 : SBAnimationSprite::LOOP_FOREVER;
    img->setAnimation(createAnimation(animType), loops);
    
    if( runAnimation ) {
        img->runAnimation([=](Node*) {
            if( onCompleted ) {
                onCompleted();
            }
        });
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
            
            feverGage.bg->setPosition(Vec2MC(getContentSize(), character.feverGagePos.x, character.feverGagePos.y));
            
        } break;
            
        case Position::RIGHT: {
            img->setFlippedX(true);
            setPosition(MAN_POS_RIGHT);
            
            feverGage.bg->setPosition(Vec2MC(getContentSize(), -character.feverGagePos.x, character.feverGagePos.y));
            
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
    
    float ratio = getFeverGageRatio();
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

float Man::getFeverGageRatio() {

    return feverGage.point / gameMgr->getConfig()->getFeverInfo().maxPoint;
}

/**
 * 승패 처리
 */
void Man::showdown(RSPResult result, RSPType myHand, RSPType oppHand) {
    
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
    
    playAttackSound();
    runAttackAnimation();
    
    setManPosition(pos);
}

/**
 * 승리!
 */
void Man::resultWin(RSPType myHand, RSPType oppHand) {
    
    playAttackSound();
    runAttackAnimation();
    
    switch( myHand ) {
        case RSPType::ROCK:     setManPosition(Position::LEFT);     break;
        case RSPType::PAPER:    break;
        case RSPType::SCISSORS: setManPosition(Position::RIGHT);    break;
        default:
            CCASSERT(false, "Man::hit error: invalid block type.");
            break;
    }
    
    // 부스트 진행중
    if( gameMgr->isBoosting() ) {
        return;
    }
    
    // fever point
    auto feverInfo = gameMgr->getConfig()->getFeverInfo();
    auto levelInfo = gameMgr->getLevelInfo();
    
    addFeverPoint(feverInfo.points[myHand] * levelInfo.feverGageRate);
    
    // 피버 게이지 초기화 스케줄러 실행
    /*
    unschedule(SCHEDULER_FEVER_GAGE_RESET);
    
    if( gameMgr->getGameMode() == GameMode::NORMAL ) {
        scheduleOnce([=](float dt) {
            this->resetFeverPoint();
        }, levelInfo.gageDuration, SCHEDULER_FEVER_GAGE_RESET);
    }
     */
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
    
    // update fever point
    // resetFeverPoint();
    setFeverPoint(feverGage.point * 0.3f, false);
    
    auto scale = ScaleTo::create(0.1f, getFeverGageRatio(), 1);
    feverGage.gage->runAction(scale);
    
    // blink
    /*
    float delay = gameMgr->getConfig()->getTimeInfo().drawDelay;
    
    auto blink = Blink::create(delay, 2);
    img->runAction(blink);
     */
    
    runAttackAnimation();
}

/**
 * 공격 효과음 재생
 */
void Man::playAttackSound() {
    
    // 기본 효과음
    if( character.punchSound == "" ) {
        ++attackSoundPlayCount;
        
        if( attackSoundPlayCount == 15 || voiceSoundIndex == SOUND_PUNCH_VOICE.size()-1 ) {
            attackSoundPlayCount = 0;

            SBAudioEngine::playEffect(SOUND_PUNCH_VOICE[voiceSoundIndex++]);
            
            if( voiceSoundIndex > SOUND_PUNCH_VOICE.size()-1 ) {
                voiceSoundIndex = 0;
            }
            
        } else {
            SBAudioEngine::playEffect(SOUND_PUNCH);
        }
    }
    // 캐릭터 효과음
    else {
        SBAudioEngine::playEffect(character.punchSound);
    }
}

/**
 * 공격 애니메이션 재생
 */
void Man::runAttackAnimation() {
    
    // run attack animation
    setAnimation(AnimationType::ATTACK, true, [=]() {
        // restore animation
        this->setAnimation(AnimationType::IDLE);
    });
}

/**
 * 사망 애니메이션 재생
 */
void Man::runDieAnimation() {
    
    // 벼락 효과음
    SBAudioEngine::playEffect(SOUND_THUNDER);
    
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
    
    auto anim = createAnimation(AnimationType::IDLE);
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
    feverGage.bg->setAnchorPoint(ANCHOR_M);
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

/**
 * 애니메이션 생성
 */
Animation* Man::createAnimation(AnimationType animType) {
    
    auto create = [](vector<string> animFiles, float delayPerUnit) -> Animation* {
        return SBNodeUtils::createAnimation(animFiles, delayPerUnit);
    };

//    auto anim = SBNodeUtils::createAnimation(ANIM_ATTACK_FILES, ANIM_ATTACK_PER_UNIT);
//    anim->setRestoreOriginalFrame(true);
//
//    img->setAnimation(anim, 1);
    
    switch( animType ) {
        case AnimationType::IDLE:     return create(character.idleAnims, HERO_ANIM_IDLE_DELAY_PER_UNIT);
        case AnimationType::ATTACK:   return create(character.attackAnims, HERO_ANIM_ATTACK_PER_UNIT);
        default:
            CCASSERT(false, "Man::createAnimation error: invalid animation.");
            break;
    }
}

