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

#define DARK_CLOUD_POS_TOP         Vec2MC(0, 660)
#define DARK_CLOUD_POS_BOTTOM      Vec2MC(0, -130)

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
    buttonLayer->showTapHint(getWinHand(blockLayer->getFirstBlock()->getType()));
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
    
    buttonLayer->showTapHint(getWinHand(blockLayer->getFirstBlock()->getType()));
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
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
        timeBar->startTimeBar();
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
    
    blockLayer->hitBlock(hitBlock, btnType, man->getManPosition());
    
    // hit 카운트 증가
    ++hitCount;
    updateScore();
    
    // 시간 증가
    timeBar->increaseTimePoint(gameMgr->getConfig()->getTimeInfo().increasePointPerHit);
    
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
    const Vec2 originPos = Vec2MC(0, 290);
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
    
    darkCloudAnim = SkeletonAnimation::createWithJsonFile(ANIM_CLOUD_DARK);
    darkCloudAnim->setAnchorPoint(Vec2::ZERO);
    darkCloudAnim->setPosition(DARK_CLOUD_POS_TOP);
    darkCloudAnim->setAnimation(0, ANIM_NAME_RUN, true);
    addChild(darkCloudAnim, (int)ZOrder::CLOUD);
    
    timeBar = TimeBar::create();
    timeBar->setAnchorPoint(ANCHOR_M);
    timeBar->setPosition(Vec2TC(0, TOP_MENU_MARGIN_Y));
    addChild(timeBar, SBZOrder::BOTTOM);
    
    timeBar->setOnTimeChangedListener([=](int timePoint, float ratio) {
        
        // 먹구름 좌표 업데이트
        float h = DARK_CLOUD_POS_TOP.y - DARK_CLOUD_POS_BOTTOM.y;
        float posY = DARK_CLOUD_POS_BOTTOM.y + (h * ratio);
        
        darkCloudAnim->setPositionY(posY);
    });
}

/**
 * 라벨 초기화
 */
void GameView::initLabels() {
    
    auto levelLabel = Label::createWithTTF("", FONT_RETRO, 75);
    levelLabel->setTag(Tag::LABEL_LEVEL);
    levelLabel->setVisible(false);
    levelLabel->setAnchorPoint(ANCHOR_M);
    levelLabel->setPosition(Vec2MC(0, 290));
    levelLabel->setColor(Color3B(250, 178, 11));
    levelLabel->enableOutline(Color4B(78, 22, 0, 255), 7);
    addChild(levelLabel, SBZOrder::BOTTOM);
    
    auto scoreLabel = Label::createWithTTF("0", FONT_RETRO, 75);
    scoreLabel->setTag(Tag::LABEL_SCORE);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(0, 170));
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 7);
    addChild(scoreLabel, SBZOrder::BOTTOM);
}

