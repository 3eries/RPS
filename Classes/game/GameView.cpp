//
//  GameView.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameView.hpp"

#include <spine/spine-cocos2dx.h>

#include "SceneManager.h"
#include "User.hpp"
#include "UIHelper.hpp"
#include "UserDefaultKey.h"

#include "GameDefine.h"

#include "object/RSPBlockLayer.hpp"
#include "object/RSPBlock.hpp"
#include "object/RSPButtonLayer.hpp"
#include "object/RSPButton.hpp"
#include "object/Man.hpp"
#include "object/TimeBar.hpp"

#include "ui/GameOverPopup.hpp"
#include "ui/PausePopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const string SCHEDULER_DRAW_DELAY = "SCHEDULER_DRAW_DELAY";

#define DARK_CLOUD_POS_TOP         Vec2MC(0, 1100)
#define DARK_CLOUD_POS_BOTTOM      Vec2MC(0, 0)
#define DARK_CLOUD_MOVE_RANGE      (DARK_CLOUD_POS_TOP.y - DARK_CLOUD_POS_BOTTOM.y)

#define LEVEL_LABEL_POS            Vec2MC(0, 290 + 130 - 30)

GameView::GameView() :
gameMgr(GameManager::getInstance()),
hitCount(0) {
}

GameView::~GameView() {
    
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    gameMgr->addListener(this);
    
    initBg();
    initBlock();
    initMan();
    
    if( SceneManager::getSceneType() == SceneType::GAME ) {
        initRSPButton();
        initTimeBar();
        initLabels();
    }
    
    return true;
}

void GameView::onEnter() {
    
    Node::onEnter();
    
    // drop 연출
    /*
    const float POS_Y = blockLayer->getPositionY();
    blockLayer->setPositionY(POS_Y + blockLayer->getContentSize().height);
    
    auto move = MoveTo::create(0.3f, Vec2(blockLayer->getPositionX(), POS_Y));
    blockLayer->runAction(move);
    */
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
    
    CCLOG("GameView onEnterTransitionDidFinish");
    
    auto cloud = getChildByTag<SkeletonAnimation*>(Tag::CLOUD);
    cloud->runAction(FadeIn::create(1.5f));
}

void GameView::onExit() {
    
    gameMgr->removeListener(this);
    
    Node::onExit();
}

void GameView::reset() {
    
    // 먹구름
    darkCloudAnim->setOpacity(0);
    
    buttonLayer->showTapHint(getWinHand(blockLayer->getFirstBlock()->getType()));
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
    
    // bgm
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        timeBar->setPositionY(Vec2TC(0, TOP_MENU_MARGIN_Y_BANNER).y);
    }
    
    // 카운트 초기화
    hitCount = 0;
    getChildByTag<Label*>(Tag::LABEL_LEVEL)->setString("");
    getChildByTag<Label*>(Tag::LABEL_SCORE)->setString("0");
    
    // showLevelLabel();
    reset();
}

/**
 * 게임 재시작
 */
void GameView::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void GameView::onGamePause() {
    
    SBNodeUtils::recursivePause(this);
}

/**
 * 게임 재게
 */
void GameView::onGameResume() {
    
    SBNodeUtils::recursiveResume(this);
}

/**
 * 게임 오버 전
 */
void GameView::onPreGameOver() {
    
    getChildByTag<Label*>(Tag::LABEL_LEVEL)->setString("");
    getChildByTag<Label*>(Tag::LABEL_SCORE)->setString("");
}

/**
 * 이어하기
 */
void GameView::onContinue() {
    
    updateScore();
    
    blockLayer->updateBlocks();
    updateButtonMode();
    
    reset();
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
}

/**
 * 타이머 시자
 */
void GameView::onStartTimer() {
    
    darkCloudAnim->stopAllActions();
    darkCloudAnim->runAction(FadeIn::create(1.0f));
    darkCloudActionTag = DARK_CLOUD_MOVE_ACTION_NONE;
    darkCloudSection = 0;
    
    updateDarkCloudPosition(0.8f, true);
}

/**
 * 게임 모드 전환
 */
void GameView::onGameModeChanged(GameMode mode) {
    
    // 피버 모드 애니메이션 초기화
    int feverModeTags [] = {
        Tag::FEVER_MODE_BG, Tag::FEVER_MODE_FIRE,
    };
    
    for( int tag : feverModeTags ) {
        auto anim = getChildByTag<SkeletonAnimation*>(tag);
        anim->setVisible(false);
        anim->clearTracks();
//        SBSpineHelper::clearAnimation(anim, ANIM_NAME_CLEAR);
    }
    
    // 모드에 따른 처리
    switch( mode ) {
        case GameMode::NORMAL: {
            SBAudioEngine::playBGM(SOUND_BGM_GAME);
        } break;
            
        case GameMode::FEVER: {
            SBAudioEngine::playBGM(SOUND_BGM_FEVER);
            
            // 피버 모드 애니메이션 재생
            for( int tag : feverModeTags ) {
                auto anim = getChildByTag<SkeletonAnimation*>(tag);
                anim->setVisible(true);
                
                anim->clearTracks();
                anim->setAnimation(0, ANIM_NAME_RUN, false);
            }
            
            // 최초 피버 모드시에만 Tap Hint 노출
            /*
            if( gameMgr->getFeverModeCount() == 1 ) {
                buttonLayer->showTapHintFeverMode();
            }
            */
            // Tap Hint 노출
            buttonLayer->showTapHintFeverMode();
            
        } break;
    }
}

/**
 * 피버 모드 종료 전
 */
void GameView::onPreFeverModeEnd() {
}

/**
 * 스코어 업데이트
 */
void GameView::updateScore() {
    
    auto saveLevelInfo = gameMgr->getLevelInfo();
    
    gameMgr->setScore(hitCount);
    getChildByTag<Label*>(Tag::LABEL_SCORE)->setString(TO_STRING(gameMgr->getScore()));
    
    // 레벨 변경된 경우, 레벨 텍스트 연출
    bool isLevelChanged = (saveLevelInfo.level != gameMgr->getLevelInfo().level);
    
    if( isLevelChanged ) {
        showLevelLabel();
    }
}

/**
 * 첫번째 블럭 타입에 따라 하단 버튼 모드를 변경
 */
void GameView::updateButtonMode() {
    
    auto block = blockLayer->getFirstBlock();
    
    if( block->getType() == RSPType::ROCK_N_ROLL ) {
        buttonLayer->switchButton(GameMode::FEVER);
    } else {
        buttonLayer->switchButton(GameMode::NORMAL);
    }
}

/**
 * 먹구름 좌표 업데이트
 * @param ratio 범위: 0(bottom) ~ 1(top)
 */
void GameView::updateDarkCloudPosition(float ratio, bool forceMove) {
    
    const float posY = DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * ratio);
    const float diff = posY - darkCloudAnim->getPositionY();
    const float diffAbs = fabsf(diff);
    
    auto stopAction = [=]() {
        if( darkCloudActionTag != DARK_CLOUD_MOVE_ACTION_NONE ) {
            darkCloudAnim->stopActionByTag(darkCloudActionTag);
        }
    };
    
    // debug label
    /*
    int DEBUG_LABEL_TAG = 130419;
    
    {
        if( !getChildByTag(DEBUG_LABEL_TAG) ) {
            auto label = Label::createWithTTF("", FONT_RETRO, 100);
            label->setTag(DEBUG_LABEL_TAG);
            label->setColor(Color3B::ORANGE);
            label->enableOutline(Color4B::BLACK);
            label->setAnchorPoint(ANCHOR_TL);
            label->setPosition(Vec2TL(100, -100));
            addChild(label, INT_MAX);
            
            for( int i = 10; i >= 0; --i ) {
                CCLOG("pos %d%% : %f", i*10,
                      (DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * i*0.1f)));
            }
        }
    }
     */
    
    auto move = [=](int section) {
        stopAction();
        
//        CCLOG("이동 section: [%d] posY: [%f]", section, posY);
        
        this->darkCloudSection = section;
        darkCloudAnim->setPositionY(posY);
        
//        getChildByTag<Label*>(DEBUG_LABEL_TAG)->setString(TO_STRING(darkCloudSection));
    };
    
    // 강제 이동
    if( forceMove ) {
        move(darkCloudSection);
        return;
    }
    
//    CCLOG("GameView::updateDarkCloudPosition ratio: [%f] diff: [%f] diffAbs: [%f]", ratio, diff, diffAbs);
    
    auto action = [=](int section, int speed) {
        if( section == darkCloudSection ) {
            // 이전과 같은 구간은 skip
            return;
        }
        
        this->darkCloudSection = section;
//        getChildByTag<Label*>(DEBUG_LABEL_TAG)->setString(TO_STRING(darkCloudSection));
        
        // 이전 액션 정지
        stopAction();
        
        // 속도 계산
        const float DURATION_PER_PX = 0.008f;    // 1픽셀 당 이동 시간
        float duration = DURATION_PER_PX;
        
        switch( speed ) {
            case 1:     duration = DURATION_PER_PX;         break;
            case 2:     duration = DURATION_PER_PX*0.8f;    break;
            case 3:     duration = DURATION_PER_PX*0.65f;   break;
            case 4:     duration = DURATION_PER_PX*0.55f;   break;
            case 5:     duration = DURATION_PER_PX*0.45f;   break;
            default:
                CCASSERT(false, "GameView::updateDarkCloudPosition::action error: invalid speed.");
                break;
        }
        
        duration *= diffAbs;
        
        // 이동 액션
//        const float fixRatio = ((int)(ratio*10) / 10.0f);
//        const float posY = DARK_CLOUD_POS_BOTTOM.y + (DARK_CLOUD_MOVE_RANGE * fixRatio);
//        CCLOG("액션 section: [%d] fixRatio: [%f] posY: [%f]", section, fixRatio, posY);
        
        // CCLOG("액션 section: [%d] posY: [%f]", section, posY);
        
        darkCloudActionTag = DARK_CLOUD_MOVE_ACTION;
        
        auto move = MoveTo::create(duration, Vec2(DARK_CLOUD_POS_TOP.x, posY));
        auto callFunc = CallFunc::create([=]() {
            darkCloudActionTag = DARK_CLOUD_MOVE_ACTION_NONE;
        });
        
        auto seq = Sequence::create(move, callFunc, nullptr);
        seq->setTag(darkCloudActionTag);
        darkCloudAnim->runAction(seq);
    };
    
    if( ratio <= 0.15f )         move(1);            // 15%
    else if( ratio <= 0.155f )   action(2, 5);       // 15.5%
    else if( ratio <= 0.2f )     action(3, 5);       // 20%
    else if( ratio <= 0.25f )    action(4, 5);       // 25%
    else if( ratio <= 0.3f )     action(5, 4);       // 30%
    else if( ratio <= 0.35f )    action(6, 4);       // 35%
    else if( ratio <= 0.4f )     action(7, 3);       // 40%
    else if( ratio <= 0.45f )    action(8, 3);       // 45%
    else if( ratio <= 0.5f )     action(9, 3);       // 50%
    else if( ratio <= 0.55f )    action(10, 3);      // 55%
    else if( ratio <= 0.6f )     action(11, 2);      // 60%
    else if( ratio <= 0.65f )    action(12, 2);      // 65%
    else if( ratio <= 0.7f )     action(13, 2);      // 70%
    else if( ratio <= 0.75f )    action(14, 1);      // 75%
    else if( ratio <= 0.8f )     action(15, 1);      // 80%
    else if( ratio <= 0.9f )     action(16, 1);      // 90%
    else                         move(17);
    
//    auto action = [=](int speed, bool isUp, int localTag) {
//        int tag = (isUp) ? DARK_CLOUD_MOVE_ACTION_UP : DARK_CLOUD_MOVE_ACTION_DOWN;
//        tag += localTag;
//
//        if( tag == darkCloudActionTag ) {
//            // 같은 동작의 액션 skip
//            return;
//        }
//
//        // 이전 액션 정지
//        stopAction();
//
//        // 속도 계산
//        // const float DURATION_PER_PX = 0.006f;    // 1픽셀 당 이동 시간
//        const float DURATION_PER_PX = 0.01f;    // 1픽셀 당 이동 시간
//        float duration = DURATION_PER_PX;
//
//        switch( speed ) {
//            case 1:     duration = DURATION_PER_PX;         break;
//            case 2:     duration = DURATION_PER_PX*0.8f;    break;
//            case 3:     duration = DURATION_PER_PX*0.7f;    break;
//            default:
//                CCASSERT(false, "GameView::updateDarkCloudPosition::action error: invalid speed.");
//                break;
//        }
//
//        duration *= diffAbs;
//
//        // 이동 액션
//        darkCloudActionTag = tag;
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
//    auto actionUp = [=](int speed, int localTag) {
//        action(speed, true, localTag);
//    };
//
//    auto actionDown = [=](int speed, int localTag) {
//        action(speed, false, localTag);
//    };
//
//    // 상승
//    if( diff > 0 ) {
//        if( ratio <= 0.1f )          actionUp(3, 1);    // 10%
//        else if( ratio <= 0.2f )     actionUp(3, 2);    // 20%
//        else if( ratio <= 0.3f )     actionUp(3, 3);    // 30%
////        else if( ratio <= 0.4f )     actionUp(2, 4);    // 40%
//        else if( ratio <= 0.5f )     actionUp(2, 5);    // 50%
////        else if( ratio <= 0.6f )     actionUp(2, 6);    // 60%
//        else if( ratio <= 0.7f )     actionUp(1, 7);    // 70%
////        else if( ratio <= 0.8f )     actionUp(1, 8);    // 80%
//        else if( ratio <= 0.9f )     actionUp(1, 9);    // 90%
//        else                         actionUp(1, 10);
//
//        /*
//        // 20%
//        if( ratio <= 0.2f ) {
//            if( diffAbs > 10 ) {
//                actionUp(3, 1);
//            }
//        }
//        // 30%
//        else if( ratio <= 0.3f ) {
//            if( diffAbs > 20 ) {
//                actionUp(3, 2);
//            }
//        }
//        // 40%
//        else if( ratio <= 0.4f ) {
//            if( diffAbs > 30 ) {
//                actionUp(2, 3);
//            }
//        }
//        // 70%
//        else if( ratio <= 0.7f ) {
//            if( diffAbs > 60 ) {
//                actionUp(1, 4);
//            }
//        }
//        // 기본
//        else {
//            if( diffAbs > 70 ) {
//                actionUp(1, 5);
//            }
//        }
//        */
//    }
//    // 하락
//    else if( diff < 0 ) {
//        // 상승중이 아니면 이동
//        if( darkCloudActionTag == DARK_CLOUD_MOVE_ACTION_NONE ||
//            darkCloudActionTag > DARK_CLOUD_MOVE_ACTION_DOWN ) {
////        if( !darkCloudAnim->getActionByTag(DARK_CLOUD_MOVE_ACTION_UP) ) {
//            CCLOG("하락!!");
//            move();
//        } else {
//            CCLOG("하락값이지만 상승 액션중임");
//        }
//
//        /*
//        // 20%
//        if( ratio <= 0.2f ) {
//            if( diffAbs > 10 ) {
//                actionDown(3, 1);
//            } else {
//                move();
//            }
//        }
//        // 30%
//        else if( ratio <= 0.3f ) {
//            if( diffAbs > 20 ) {
//                actionDown(3, 2);
//            } else {
//                move();
//            }
//        }
//        // 40%
//        else if( ratio <= 0.4f ) {
//            if( diffAbs > 30 ) {
//                actionDown(2, 3);
//            }
//        }
//        // 70%
//        else if( ratio <= 0.7f ) {
//            if( diffAbs > 60 ) {
//                actionDown(1, 4);
//            }
//        }
//        // 기본
//        else {
//            if( diffAbs > 70 ) {
//                actionDown(1, 5);
//            }
//        }
//        */
//    }
}

/**
 * RSP 버튼 클릭
 */
void GameView::onClickNormalButton(RSPType type) {
    
    auto block = blockLayer->getFirstBlock();
    auto result = getResult(type, block->getType());
    
    man->showdown(result, type, block->getType());
    
    switch( result ) {
        case RSPResult::WIN:    this->hitBlock(block, type);      break;
        case RSPResult::LOSE:   this->misBlock(block);            break;
        case RSPResult::DRAW:   this->drawBlock(block);           break;
        default: break;
    }
    
    // 타임바 실행
    if( !timeBar->isStarted() && result != RSPResult::LOSE ) {
        gameMgr->onStartTimer();
    }
}

/**
 * 피버 버튼 클릭
 */
void GameView::onClickFeverButton(int i) {
    
    auto block = blockLayer->getFirstBlock();
    man->rockNroll((i == 0) ? Man::Position::LEFT : Man::Position::RIGHT);
    
    hitBlock(block, RSPType::ROCK_N_ROLL);
}

/**
 *  블럭 히트다 히트~!
 */
void GameView::hitBlock(RSPBlock *hitBlock, RSPType btnType) {
    
    CCLOG("GameView::hitBlock gameMode: %d", (int)gameMgr->getGameMode());
    
    auto hitBlockType = hitBlock->getType();
    blockLayer->hitBlock(hitBlock, btnType, man->getManPosition());
    
    // hit 카운트 증가
    ++hitCount;
    updateScore();
    
    // 시간 증가
    auto levelInfo = gameMgr->getLevelInfo();
    float increasePoint = levelInfo.increasePointPerHit;
    
    if( hitBlockType == RSPType::ROCK_N_ROLL ) {
        // 피버 블럭
        increasePoint = levelInfo.increasePointPerFeverHit;
    }
    
    timeBar->increaseTimePoint(increasePoint);
    
    // 버튼 업데이트
    updateButtonMode();
}

/**
 *  블럭 틀렸다잉
 */
void GameView::misBlock(RSPBlock *block) {
 
    blockLayer->misBlock(block);
    
    gameMgr->onPreGameOver();
}

/**
 * 비겼당
 */
void GameView::drawBlock(RSPBlock *block) {
    
    // vibrate
    Device::vibrate(DRAW_VIBRATE_DURATION);
    
    // 블럭 연출
    blockLayer->drawBlock(block);
    
    const Vec2 originManPos = man->getPosition();
    block->runDrawAnimation(man->isPositionLeft(), [=](int i) {
        
        // 캐릭터 이동
        // 할아범은 총 3번 진동하는데, draw애니메이션이 시작된후
        // 0.033초후에 20px뒤로 밀려나고
        // 0.66초 후에 제자리로 돌아오고
        // 0.1초후에 다시 20px뒤로 밀려나고
        // 0.133초후에 제자리로 돌아오고
        // 0.166초후에 20px밀려났다가
        // 0.2초에 다시 제자리로 돌아옵니당
        
        // move_1 ~ move_6
        float moveX[] = {
            -20, 0, -20, 0, -20, 0
        };
        
        float x = originManPos.x;
        x += man->isPositionLeft() ? moveX[i-1] : -moveX[i-1];
        
//        man->setPositionX(x);
        
        man->stopActionByTag(Man::ACTION_TAG_DRAW_MOVE);
        
        auto move = MoveTo::create(0.03f, Vec2(x, originManPos.y));
        move->setTag(Man::ACTION_TAG_DRAW_MOVE);
        man->runAction(move);
    });
    
    // 버튼 터치 지연
    buttonLayer->touchLocked(gameMgr->getConfig()->getTimeInfo().drawDelay);
}

/**
 * Level 라벨 노출
 */
void GameView::showLevelLabel() {
    
    auto levelLabel = getChildByTag<Label*>(Tag::LABEL_LEVEL);
    levelLabel->setVisible(true);
    
    levelLabel->stopAllActions();
    levelLabel->setString(STR_FORMAT("LEVEL %d", gameMgr->getLevelInfo().level));
    
    const Vec2 topPos    = Vec2TC(0, levelLabel->getContentSize().height);
    const Vec2 originPos = LEVEL_LABEL_POS;
    levelLabel->setPosition(topPos);
    
    // move
    auto move1 = MoveTo::create(0.2f, originPos);
    auto move2 = MoveTo::create(0.2f, topPos);
    auto delay = DelayTime::create(1.0f);
    levelLabel->runAction(Sequence::create(move1, delay, move2, nullptr));
    
    // n초 후 hide
//    auto delay = DelayTime::create(1.0f);
//    auto hide = Hide::create();
//    levelLabel->runAction(Sequence::create(delay, hide, nullptr));
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
    
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_bg.png");
    bg->setTag(Tag::BG);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0,0));
    addChild(bg, (int)ZOrder::BG);
    
    // cloud
    auto cloud = SkeletonAnimation::createWithJsonFile(ANIM_CLOUD);
    cloud->setTag(Tag::CLOUD);
    cloud->setAnchorPoint(Vec2::ZERO);
    cloud->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    cloud->setAnimation(0, ANIM_NAME_RUN, true);
    cloud->update(0);
    cloud->setOpacity(0);
    addChild(cloud, (int)ZOrder::CLOUD);

    // dark cloud
    if( SceneManager::getSceneType() == SceneType::GAME ) {
        darkCloudAnim = SkeletonAnimation::createWithJsonFile(ANIM_CLOUD_DARK);
        darkCloudAnim->setAnchorPoint(Vec2::ZERO);
        darkCloudAnim->setPosition(DARK_CLOUD_POS_TOP);
        darkCloudAnim->setAnimation(0, ANIM_NAME_RUN, true);
        darkCloudAnim->setOpacity(0);
        darkCloudAnim->setVisible(UserDefault::getInstance()->getBoolForKey(UserDefaultKey::TEST_DARK_CLOUD_ENABLED, true));
        addChild(darkCloudAnim, (int)ZOrder::CLOUD);
    }
    
    // 피버 모드
    {
        // 배경
        auto feverModeBg = SkeletonAnimation::createWithJsonFile(ANIM_FEVER_MODE_BG);
        feverModeBg->setTag(Tag::FEVER_MODE_BG);
        feverModeBg->setAnchorPoint(Vec2::ZERO);
        feverModeBg->setPosition(Vec2(SB_WIN_SIZE*0.5f));
        addChild(feverModeBg, (int)ZOrder::BG);
        
        feverModeBg->setVisible(false);
//        SBSpineHelper::clearAnimation(feverModeBg, ANIM_NAME_CLEAR);
        
        // Fire!
        auto feverModeFire = SkeletonAnimation::createWithJsonFile(ANIM_FEVER_MODE_FIRE);
        feverModeFire->setTag(Tag::FEVER_MODE_FIRE);
        feverModeFire->setAnchorPoint(Vec2::ZERO);
        feverModeFire->setPosition(Vec2(SB_WIN_SIZE*0.5f));
        addChild(feverModeFire, (int)ZOrder::CLOUD);
        
        feverModeFire->setVisible(false);
//        SBSpineHelper::clearAnimation(feverModeFire, ANIM_NAME_CLEAR);
    }
    
    // 바닥 기둥
    auto bottomBg = Sprite::create(DIR_IMG_GAME + "RSP_bg_bottom.png");
    bottomBg->setAnchorPoint(ANCHOR_MB);
    bottomBg->setPosition(Vec2BC(0,0));
    addChild(bottomBg, (int)ZOrder::CLOUD);
}

/**
 * RSP 블럭 초기화
 */
void GameView::initBlock() {
    
    blockLayer = RSPBlockLayer::create();
    addChild(blockLayer, (int)ZOrder::BLOCK);
}

/**
 * RSP 버튼 초기화
 */
void GameView::initRSPButton() {
    
    buttonLayer = RSPButtonLayer::create();
    addChild(buttonLayer);
    
    buttonLayer->setOnNormalButtonClickListener([=](RSPType type) {
        this->onClickNormalButton(type);
    });
    
    buttonLayer->setOnFeverButtonClickListener([=](int i) {
        this->onClickFeverButton(i);
    });
}

/**
 * Man 초기화
 */
void GameView::initMan() {
    
    man = Man::create();
    addChild(man);
}

/**
 * TimeBar 초기화
 */
void GameView::initTimeBar() {
    
    timeBar = TimeBar::create();
    timeBar->setAnchorPoint(ANCHOR_M);
    timeBar->setPosition(Vec2TC(0, TOP_MENU_MARGIN_Y));
    addChild(timeBar, SBZOrder::BOTTOM);
    
    timeBar->setOnTimeChangedListener([=](float timePoint, float ratio) {
        // 먹구름 좌표 업데이트
        this->updateDarkCloudPosition(ratio);
    });
}

/**
 * 라벨 초기화
 */
void GameView::initLabels() {
    
    auto levelLabel = Label::createWithTTF("", FONT_RETRO, 70);
    levelLabel->setTag(Tag::LABEL_LEVEL);
    levelLabel->setVisible(false);
    levelLabel->setAnchorPoint(ANCHOR_M);
    levelLabel->setPosition(LEVEL_LABEL_POS);
    levelLabel->setColor(Color3B(250, 178, 11));
    levelLabel->enableOutline(Color4B(78, 22, 0, 255), 5);
    addChild(levelLabel, SBZOrder::BOTTOM);
    
    auto scoreLabel = Label::createWithTTF("0", FONT_RETRO, 70);
    scoreLabel->setTag(Tag::LABEL_SCORE);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(0, 170 + 130 - 10));
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 5);
    addChild(scoreLabel, SBZOrder::BOTTOM);
}

