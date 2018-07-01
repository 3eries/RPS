//
//  DarkCloud.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#include "DarkCloud.hpp"

#include "RSP.h"
#include "UserDefaultKey.h"

#include "TimeBar.hpp"

USING_NS_CC;
using namespace spine;
using namespace std;

#define TIME_RATIO_SCALE           0.5f
#define OPACITY_RATIO_SCALE        0.6f

#define DARK_CLOUD_POS_TOP         Vec2MC(0, 1100 * TIME_RATIO_SCALE)
#define DARK_CLOUD_POS_BOTTOM      Vec2MC(0, 0)
#define DARK_CLOUD_MOVE_RANGE      (DARK_CLOUD_POS_TOP.y - DARK_CLOUD_POS_BOTTOM.y)

#define DEBUG_DRAW                 1

DarkCloud::DarkCloud() :
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
    
    anim = SkeletonAnimation::createWithJsonFile(ANIM_CLOUD_DARK);
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(DARK_CLOUD_POS_TOP);
    anim->setAnimation(0, ANIM_NAME_RUN, true);
    anim->setVisible(false);
    addChild(anim);
    
    setVisible(UserDefault::getInstance()->getBoolForKey(UserDefaultKey::TEST_DARK_CLOUD_ENABLED, true));
    
    GameManager::getInstance()->addListener(this);
    
#if (DEBUG_DRAW)
    
    for( int i = 0; i <= 10; ++i ) {
        float ratio = i * 0.1f;
        float posY = getCloudPositionY(ratio) - (420);
        
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
    
#endif
    
    return true;
}

void DarkCloud::onExit() {
    
    GameManager::getInstance()->removeListener(this);
    
    Node::onExit();
}

void DarkCloud::reset() {
    
    anim->stopAllActions();
    anim->setOpacity(0);
    anim->setVisible(false);
    
    isPositionLocked = false;
    isOpacityLocked = false;
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

}

/**
 * 타이머 시작
 */
void DarkCloud::onStartTimer() {

    actionTag = MOVE_ACTION_TAG_NONE;
    section = 0;
    
    const float timeRatio = timeBar->getTimeRatio();
    updateCloud(timeRatio);
    
    // fade in
    anim->stopAllActions();
    anim->setOpacity(0);
    anim->setVisible(true);
    
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
 * 먹구름 업데이트
 */
void DarkCloud::updateCloud(float timeRatio) {
    
    timeRatio *= (1 / TIME_RATIO_SCALE);
    timeRatio = MIN(1, timeRatio);
    this->timeRatio = timeRatio;
    
    updateCloudPosition(timeRatio);
    updateCloudOpacity(timeRatio);
}

/**
 * 좌표 업데이트
 */
void DarkCloud::updateCloudPosition(float timeRatio) {
    
    if( isPositionLocked ) {
        return;
    }
    
    anim->stopActionByTag(ActionTag::MOVE);
    
    const float posY = getCloudPositionY(timeRatio);
    const float diff = posY - anim->getPositionY();
    const float diffAbs = fabsf(diff);
    
    anim->setPositionY(posY);

    return;
    
    // 이동 액션
    if( diffAbs > 10 ) {
        isPositionLocked = true;
        
        const float DURATION_PER_PX = 0.008f;    // 1픽셀 당 이동 시간
        const float duration = DURATION_PER_PX;
        
        auto move = MoveTo::create(duration, Vec2(DARK_CLOUD_POS_TOP.x, posY));
        auto callFunc = CallFunc::create([=]() {
            isPositionLocked = false;
        });

        auto seq = Sequence::create(move, callFunc, nullptr);
        seq->setTag(ActionTag::MOVE);
        anim->runAction(seq);
    }
    // 바로 이동
    else {
        anim->setPositionY(posY);
    }
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

float DarkCloud::getCloudPositionY(float timeRatio) {
    
    return DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * timeRatio);
}

float DarkCloud::getCloudOpacity(float timeRatio) {
    
//    (0.9-0.6) / (0.4)
    timeRatio = (timeRatio-OPACITY_RATIO_SCALE) / (1-OPACITY_RATIO_SCALE);
    timeRatio = MAX(0, timeRatio);
    
    float f = 1-timeRatio;
    float opacity = 255 * f;
//    float opacity = 255 * (f*f);
    
//    CCLOG("opacity ratio: [%f] origin: [%2.0f], result: [%2.0f]", timeRatio,
//          (255 - (255 * timeRatio)), opacity);
    
    return opacity;
}

/**
 * 먹구름 좌표 업데이트
 * @param ratio 범위: 0(bottom) ~ 1(top)
 */
//void GameView::updateDarkCloudPosition(float ratio, bool forceMove) {
//
//    const float posY = DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * ratio);
//    const float diff = posY - darkCloudAnim->getPositionY();
//    const float diffAbs = fabsf(diff);
//
//    // 투명도
//    {
//        darkCloudAnim->setOpacity(255 - (255 * ratio));
//    }
//
//    darkCloudAnim->setPositionY(posY);
//
//    return;
//
//    auto stopAction = [=]() {
//        if( darkCloudActionTag != DARK_CLOUD_MOVE_ACTION_NONE ) {
//            darkCloudAnim->stopActionByTag(darkCloudActionTag);
//        }
//    };
//
//    // debug label
//    /*
//     int DEBUG_LABEL_TAG = 130419;
//
//     {
//     if( !getChildByTag(DEBUG_LABEL_TAG) ) {
//     auto label = Label::createWithTTF("", FONT_RETRO, 100);
//     label->setTag(DEBUG_LABEL_TAG);
//     label->setColor(Color3B::ORANGE);
//     label->enableOutline(Color4B::BLACK);
//     label->setAnchorPoint(ANCHOR_TL);
//     label->setPosition(Vec2TL(100, -100));
//     addChild(label, INT_MAX);
//
//     for( int i = 10; i >= 0; --i ) {
//     CCLOG("pos %d%% : %f", i*10,
//     (DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * i*0.1f)));
//     }
//     }
//     }
//     */
//
//    auto move = [=](int section) {
//        stopAction();
//
//        //        CCLOG("이동 section: [%d] posY: [%f]", section, posY);
//
//        this->darkCloudSection = section;
//        darkCloudAnim->setPositionY(posY);
//
//        //        getChildByTag<Label*>(DEBUG_LABEL_TAG)->setString(TO_STRING(darkCloudSection));
//    };
//
//    // 강제 이동
//    if( forceMove ) {
//        move(darkCloudSection);
//        return;
//    }
//
//    //    CCLOG("GameView::updateDarkCloudPosition ratio: [%f] diff: [%f] diffAbs: [%f]", ratio, diff, diffAbs);
//
//    auto action = [=](int section, int speed) {
//        if( section == darkCloudSection ) {
//            // 이전과 같은 구간은 skip
//            return;
//        }
//
//        this->darkCloudSection = section;
//        //        getChildByTag<Label*>(DEBUG_LABEL_TAG)->setString(TO_STRING(darkCloudSection));
//
//        // 이전 액션 정지
//        stopAction();
//
//        // 속도 계산
//        const float DURATION_PER_PX = 0.008f;    // 1픽셀 당 이동 시간
//        float duration = DURATION_PER_PX;
//
//        switch( speed ) {
//            case 1:     duration = DURATION_PER_PX;         break;
//            case 2:     duration = DURATION_PER_PX*0.8f;    break;
//            case 3:     duration = DURATION_PER_PX*0.65f;   break;
//            case 4:     duration = DURATION_PER_PX*0.55f;   break;
//            case 5:     duration = DURATION_PER_PX*0.45f;   break;
//            default:
//                CCASSERT(false, "GameView::updateDarkCloudPosition::action error: invalid speed.");
//                break;
//        }
//
//        duration *= diffAbs;
//
//        // 이동 액션
//        //        const float fixRatio = ((int)(ratio*10) / 10.0f);
//        //        const float posY = DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * fixRatio);
//        //        CCLOG("액션 section: [%d] fixRatio: [%f] posY: [%f]", section, fixRatio, posY);
//
//        // CCLOG("액션 section: [%d] posY: [%f]", section, posY);
//
//        darkCloudActionTag = DARK_CLOUD_MOVE_ACTION;
//
//        auto move = MoveTo::create(duration, Vec2(DARK_CLOUD_POS_TOP.x, posY));
//        auto callFunc = CallFunc::create([=]() {
//            darkCloudActionTag = DARK_CLOUD_MOVE_ACTION_NONE;
//        });
//
//        auto seq = Sequence::create(move, callFunc, nullptr);
//        seq->setTag(darkCloudActionTag);
//        darkCloudAnim->runAction(seq);
//    };
//
//    if( ratio <= 0.15f )         move(1);            // 15%
//    else if( ratio <= 0.155f )   action(2, 5);       // 15.5%
//    else if( ratio <= 0.2f )     action(3, 5);       // 20%
//    else if( ratio <= 0.25f )    action(4, 5);       // 25%
//    else if( ratio <= 0.3f )     action(5, 4);       // 30%
//    else if( ratio <= 0.35f )    action(6, 4);       // 35%
//    else if( ratio <= 0.4f )     action(7, 3);       // 40%
//    else if( ratio <= 0.45f )    action(8, 3);       // 45%
//    else if( ratio <= 0.5f )     action(9, 3);       // 50%
//    else if( ratio <= 0.55f )    action(10, 3);      // 55%
//    else if( ratio <= 0.6f )     action(11, 2);      // 60%
//    else if( ratio <= 0.65f )    action(12, 2);      // 65%
//    else if( ratio <= 0.7f )     action(13, 2);      // 70%
//    else if( ratio <= 0.75f )    action(14, 1);      // 75%
//    else if( ratio <= 0.8f )     action(15, 1);      // 80%
//    else if( ratio <= 0.9f )     action(16, 1);      // 90%
//    else                         move(17);
//}
