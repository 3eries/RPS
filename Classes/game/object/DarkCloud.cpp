//
//  DarkCloud.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#include "DarkCloud.hpp"

#include "Define.h"
#include "UserDefaultKey.h"
#include "TestHelper.hpp"

#include "TimeBar.hpp"

USING_NS_CC;
using namespace spine;
using namespace std;

// 타임 게이지 스케일 값
#define TIME_RATIO_SCALE                     0.5f

// 먹구름 불투명도 스케일 값
// 먹구름 게이지 100 ~ 60% -> 불투명도 0 ~ 100%
#define OPACITY_RATIO_SCALE                  0.6f

// 데드라인 불투명도, 먹구름 기준의 비율 값
// 먹구름 50 ~ 30% -> 불투명도 0 ~ 100%
#define DEAD_LINE_OPACITY_RATIO_MAX          0.5f   // 시작 (0%)
#define DEAD_LINE_OPACITY_RATIO_MIN          0.3f   // 종료 (100%)
#define DEAD_LINE_OPACITY_RATIO_RANGE        (DEAD_LINE_OPACITY_RATIO_MAX - DEAD_LINE_OPACITY_RATIO_MIN)

//#define DARK_CLOUD_POS_TOP         Vec2MC(0, 1100 * TIME_RATIO_SCALE)
#define DARK_CLOUD_POS_TOP         Vec2MC(0, 1100)
#define DARK_CLOUD_POS_BOTTOM      Vec2MC(0, 293)
#define DARK_CLOUD_MOVE_RANGE      (DARK_CLOUD_POS_TOP.y - DARK_CLOUD_POS_BOTTOM.y)

#define DEBUG_DRAW                 0
#define DEBUG_LABEL_TAG            130419

static const string SCHEDULER_POSITION_LOCKED   = "SCHEDULER_POSITION_LOCKED";
static const string SCHEDULER_POSITION_UNLOCKED = "SCHEDULER_POSITION_UNLOCKED";

DarkCloud::DarkCloud() :
gameMgr(GameManager::getInstance()),
timeRatio(0),
isPositionLocked(false),
isOpacityLocked(false) {
    
}

DarkCloud::~DarkCloud() {
    
}

bool DarkCloud::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    // animation
    anim = SkeletonAnimation::createWithJsonFile(ANIM_CLOUD_DARK);
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(DARK_CLOUD_POS_TOP);
    anim->setAnimation(0, ANIM_NAME_RUN, true);
    anim->setVisible(false);
    addChild(anim);
    
    // dead line
    deadLine = Sprite::create(DIR_IMG_GAME + "RSP_line_die.png");
    deadLine->setAnchorPoint(ANCHOR_M);
    deadLine->setPosition(Vec2MC(0, -138));
    deadLine->setVisible(false);
    addChild(deadLine);
    
    // listener
    gameMgr->addListener(this);
    
#if (DEBUG_DRAW)
    
    // line
    for( int i = 0; i <= 10; ++i ) {
        float ratio = i * 0.1f;
        float posY = getCloudPosition(ratio) - (420);
        
        auto line = LayerColor::create(Color4B(255,0,0,255*0.7f));
        line->setIgnoreAnchorPointForPosition(false);
        line->setAnchorPoint(ANCHOR_ML);
        line->setPosition(Vec2(0, posY));
        line->setContentSize(Size(SB_WIN_SIZE.width, 10));
        addChild(line, SBZOrder::TOP);
        
        auto title = Label::createWithTTF(STR_FORMAT("%.1f", ratio), FONT_RETRO, 40);
        title->setColor(Color3B::WHITE);
        title->enableOutline(Color4B::BLACK);
        title->setAnchorPoint(ANCHOR_ML);
        title->setPosition(Vec2(100, posY));
        addChild(title, SBZOrder::TOP);
    }
    
    auto label = Label::createWithTTF("", FONT_RETRO, 50);
    label->setTag(DEBUG_LABEL_TAG);
    label->setColor(Color3B::ORANGE);
    label->enableOutline(Color4B::BLACK);
    label->setAnchorPoint(ANCHOR_TL);
    label->setPosition(Vec2TL(30, -250));
    addChild(label, INT_MAX);
    
#endif
    
    return true;
}

void DarkCloud::onExit() {
    
    gameMgr->removeListener(this);
    
    Node::onExit();
}

void DarkCloud::reset() {
    
    setVisible(true);
    
#if ENABLE_TEST_MENU
    setVisible(TestHelper::getInstance()->isDarkCloudEnabled());
#endif
    
    anim->stopAllActions();
    anim->setOpacity(0);
    anim->setVisible(false);
    
    deadLine->setOpacity(0);
    deadLine->setVisible(false);
    
    isPositionLocked = false;
    isOpacityLocked = false;
    
    unscheduleUpdate();
}

/**
 * 게임 시작
 */
void DarkCloud::onGameStart() {
    
    reset();
}

void DarkCloud::onGameRestart() {
}

void DarkCloud::onGamePause() {
}

void DarkCloud::onGameResume() {
}

/**
 * 게임 오버 전
 */
void DarkCloud::onPreGameOver() {
}

/**
 * 이어하기
 */
void DarkCloud::onContinue() {
    
    reset();
}

void DarkCloud::onGameOver() {

    // setVisible(false);
}

/**
 * 타이머 시작
 */
void DarkCloud::onStartTimer() {

    isPositionLocked = false;
    isOpacityLocked = false;
    targetPosition = 0;
    
    const float timeRatio = timeBar->getTimeRatio();
    updateCloud(timeRatio, true);
    
    // fade in
    anim->stopAllActions();
    anim->setOpacity(0);
    anim->setVisible(true);
    
    // deadLine->setOpacity(0);
    deadLine->setVisible(true);
    
    scheduleUpdate();
    
    /*
    isOpacityLocked = true;
    
    auto fadeIn = FadeTo::create(1.0f, getCloudOpacity(timeRatio));
    auto callFunc = CallFunc::create([=]() {
        isOpacityLocked = false;
    });
    
    auto seq = Sequence::create(fadeIn, callFunc, nullptr);
    anim->runAction(seq);
     */
    
    // update position
//    updateCloudPosition(timeRatio);
}

/**
 * 레벨 변경
 */
void DarkCloud::onLevelChanged(LevelInfo level) {
    
}

/**
 * 먹구름 업데이트
 */
#define TEST 0
void DarkCloud::update(float dt) {
 
    auto updatePosition = [=]() {
    
        // 좌표 잠금
        if( this->isPositionLocked ) {
            return;
        }
        
        const float oldPos = anim->getPositionY();
        float diff = targetPosition - oldPos;
        float diffAbs = fabsf(diff);
        
        // 좌표 변경 없음
        if( diff == 0 ) {
            return;
        }
        
        // 상태 초기화
        auto unlockPosition = [=]() {
            this->unschedule(SCHEDULER_POSITION_LOCKED);
            this->unschedule(SCHEDULER_POSITION_UNLOCKED);
            isPositionLocked = false;
        };
        
        unlockPosition();
        
        // 시간 포인트 계산
        const float MAX_POINT = gameMgr->getConfig()->getTimeInfo().maxPoint * TIME_RATIO_SCALE;
        
        // 1초 당 증가 포인트 비율, 초당 4회 히트로 계산
        /*
        const float INCREASE_POINT = gameMgr->getLevelInfo().increasePointPerHit * 4;
        const float INCREASE_RATIO = INCREASE_POINT / MAX_POINT;
        const float INCREASE_MOVE_RANGE = DARK_CLOUD_MOVE_RANGE * INCREASE_RATIO; // 1초 당 이동 범위
         */
        
        // 1초 당 감소 포인트 비율
        const float DECREASE_POINT = gameMgr->getLevelInfo().decreasePointPerSeconds;
        const float DECREASE_RATIO = DECREASE_POINT / MAX_POINT;
        // const float DECREASE_MOVE_RANGE = DARK_CLOUD_MOVE_RANGE * DECREASE_RATIO; // 1초 당 이동 범위
        
        // 이동 범위 (기준 : dt)
        const float DECREASE_MOVE_RANGE = (DARK_CLOUD_MOVE_RANGE * DECREASE_RATIO) * dt;
        
        auto setPosition = [=](float diff) {
            anim->setPositionY(oldPos + diff);
        };
        
        // 상승
        if( diff > 0 ) {
            const float MIN_MOVE_DIST = DARK_CLOUD_MOVE_RANGE / 20; // 최소 이동 거리
            
            if( diffAbs < MIN_MOVE_DIST ) {
                // 이동 거리 짧음
                return;
            }
            
            diff = MIN(DECREASE_MOVE_RANGE, diff); // tuning
            setPosition(diff);
            
#if (DEBUG_DRAW)
            string str = STR_FORMAT("%.2f\n(%.2f)", (targetPosition - oldPos), diff);
            CCLOG("상승 diff: %s", str.c_str());
            
            getChildByTag<Label*>(DEBUG_LABEL_TAG)->setString(str);
#endif
            
            // 다음 이동 지연
            isPositionLocked = true;
            
            this->schedule([=](float dt) {
                
                if( !isPositionLocked ) {
                    return;
                }
                
                float diff = targetPosition - anim->getPositionY();
                
                // 충분히 이동함
                if( diff >= MIN_MOVE_DIST ) {
                    unlockPosition();
                }
                // 하락으로 변경됨
                else if( diff < 0 ) {
                    unlockPosition();
                }
                
            }, SCHEDULER_POSITION_LOCKED);

            /*
            this->scheduleOnce([=](float dt) {
                if( isPositionLocked ) {
                    unlockPosition();
                }
            }, 0.05f, SCHEDULER_POSITION_UNLOCKED);
             */
            this->schedule([=](float dt) {
                if( isPositionLocked ) {
                    unlockPosition();
                }
            }, 0.05f, SCHEDULER_POSITION_UNLOCKED);
        }
        // 하락
        else if( diff < 0 ) {
//            diff = MAX(-DECREASE_MOVE_RANGE, diff);
            setPosition(diff);
        }
    
        return;
        
#if (TEST)
        auto setPosition = [=](float ratio) {

            float moveRange = DARK_CLOUD_MOVE_RANGE * ratio; // 1초 당 이동 범위
            
            float pos  = oldPos + (moveRange * (diff > 0 ? dt : -dt));
            anim->setPositionY(pos);
        };
        
        // 상승
        if( diff > 0 ) {
            setPosition(INCREASE_RATIO);
            
            // 다음 이동 지연
            auto unlockPosition = [=]() {
                this->unschedule(SCHEDULER_POSITION_LOCKED);
                this->unschedule(SCHEDULER_POSITION_UNLOCKED);
                isPositionLocked = false;
            };
            
            unlockPosition();
            isPositionLocked = true;
            
            this->schedule([=](float dt) {
                
                if( !isPositionLocked ) {
                    return;
                }
                
                float diff = targetPosition - anim->getPositionY();
        
                // 하락으로 변경됨
                if( diff < 0 ) {
                    unlockPosition();
                }
                
            }, SCHEDULER_POSITION_LOCKED);

            this->scheduleOnce([=](float dt) {
                if( isPositionLocked ) {
                    unlockPosition();
                }
            }, 0.1f, SCHEDULER_POSITION_UNLOCKED);
            
            /*
            float delayTime = (INCREASE_POINT / DECREASE_POINT) * 0.5f;
            
            if( delayTime < dt ) {
                return;
            }
            
            isPositionLocked = true;
            
            auto delay = DelayTime::create(delayTime);
            auto callFunc = CallFunc::create([=]() {
                isPositionLocked = false;
            });
            
            auto seq = Sequence::create(delay, callFunc, nullptr);
            // seq->setTag(ActionTag::MOVE);
            anim->runAction(seq);
             */
        }
        // 하락
        else if( diff < 0 ) {
            setPosition(DECREASE_RATIO);
        }
#endif
    };
    
    // 좌표 업데이트
    updatePosition();
}

/**
 * 시간 비율 업데이트
 */
void DarkCloud::updateCloud(float timeRatio, bool forceUpdate) {
    
    // 데드라인 업데이트
    updateDeadLine(timeRatio);
    
    // 좌표, 투명도에 적용할 시간 비율 조정
    timeRatio *= (1 / TIME_RATIO_SCALE);
    timeRatio = MIN(1, timeRatio);
    this->timeRatio = timeRatio;
    
#if (DEBUG_DRAW)
    // getChildByTag<Label*>(DEBUG_LABEL_TAG)->setString(STR_FORMAT("%.3f", this->timeRatio));
#endif
    
    updateCloudPosition(timeRatio, forceUpdate);
    updateCloudOpacity(timeRatio);
}

/**
 * 좌표 업데이트
 */
void DarkCloud::updateCloudPosition(float timeRatio, bool forceMove) {
    
    const float posY = getCloudPosition(timeRatio);

    if( forceMove || timeRatio == 1 ) {
        anim->setPositionY(posY);
    }
    
    targetPosition = posY;
}

/**
 * 투명도 업데이트
 */
void DarkCloud::updateCloudOpacity(float timeRatio) {

    if( isOpacityLocked ) {
        return;
    }
    
    anim->setOpacity(getCloudOpacity(timeRatio));
}

/**
 * 데드라인 업데이트
 */
void DarkCloud::updateDeadLine(float timeRatio) {

    if( deadLine->getActionByTag(DEAD_LINE_ACTION_TAG) ) {
        // 이미 액션중
        return;
    }
    
    // show
    if( timeRatio < 0.35f ) {
        if( deadLine->getOpacity() != 255 ) {
            auto fadeIn = FadeIn::create(0.4f);
            auto delay = DelayTime::create(0.5f);
            
            auto seq = Sequence::create(fadeIn, delay, nullptr);
            seq->setTag(DEAD_LINE_ACTION_TAG);
            deadLine->runAction(seq);
        }
    }
    // hide
    else if( timeRatio > 0.4f ) {
        if( deadLine->getOpacity() != 0 ) {
            auto fadeOut = FadeOut::create(0.4f);
            auto delay = DelayTime::create(0.5f);
            
            auto seq = Sequence::create(fadeOut, delay, nullptr);
            seq->setTag(DEAD_LINE_ACTION_TAG);
            deadLine->runAction(seq);
        }
    }
    
    // deadLine->setOpacity(getDeadLineOpacity(timeRatio));
}

float DarkCloud::getCloudPosition(float timeRatio) {
    
    return DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * timeRatio);
}

float DarkCloud::getCloudOpacity(float timeRatio) {
    
    timeRatio = (timeRatio - OPACITY_RATIO_SCALE) / (1 - OPACITY_RATIO_SCALE);
    timeRatio = MIN(1, timeRatio);
    timeRatio = MAX(0, timeRatio);
    
    float f = 1 - timeRatio;
//    float opacity = 255 * f;
    float opacity = 255 * (f*f);
    
    return opacity;
}

float DarkCloud::getDeadLineOpacity(float timeRatio) {
    
    // (0.25 - 0.2) / (0.4 - 0.2)
    timeRatio = (timeRatio - DEAD_LINE_OPACITY_RATIO_MIN) / DEAD_LINE_OPACITY_RATIO_RANGE;
    timeRatio = MIN(1, timeRatio);
    timeRatio = MAX(0, timeRatio);
    
    float f = 1 - timeRatio;
    float opacity = 255 * (f*f);
    
    return opacity;
}


