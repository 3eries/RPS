//
//  GameView.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameView.hpp"

#include "SceneManager.h"
#include "User.hpp"
#include "UIHelper.hpp"

#include "GameDefine.h"

#include "object/RSPBlock.hpp"
#include "object/RSPButtonLayer.hpp"
#include "object/RSPButton.hpp"
#include "object/Man.hpp"
#include "object/TimeBar.hpp"

#include "ui/GameOverPopup.hpp"
#include "ui/PausePopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const string SCHEDULER_DRAW_DELAY = "SCHEDULER_DRAW_DELAY";

GameView::GameView() :
gameMgr(GameManager::getInstance()),
blockLayer(nullptr),
blockIndex(0),
hitCount(0) {
}

GameView::~GameView() {
    
    blocks.clear();
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    initBlocks();
    initRSPButton();
    initMan();
    initTimeBar();
    initLabels();
    initMenu();
    
    ViewManager::getInstance()->addListener(this);
    gameMgr->addListener(this);
    gameMgr->onEnterGame(this);
    
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
}

void GameView::onExit() {
    
    ViewManager::getInstance()->removeListener(this);
    gameMgr->removeListener(this);
    
    Node::onExit();
}

/**
 * 뷰 타입 변경
 */
void GameView::onViewChanged(ViewType viewType) {
    
    auto setGameNodeVisible = [=](bool visible) {
        for( auto n : gameNodes ) {
            n->setVisible(visible);
        }
    };
    
    switch( viewType ) {
        case ViewType::MAIN: {
            setGameNodeVisible(false);
        } break;
            
        case ViewType::GAME: {
            setGameNodeVisible(true);
            
            gameMgr->onGameStart();
            
        } break;
            
        default:
            break;
    }
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
    
    // bgm
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        timeBar->setPosition(Vec2TC(0, -BANNER_HEIGHT-10));
        
        auto menuLayer = getChildByTag(Tag::LAYER_MENU);
        menuLayer->getChildByTag(Tag::BTN_PAUSE)->setPosition(Vec2TR(-10, -BANNER_HEIGHT-10));
    }
    
    // 카운트 초기화
    hitCount = 0;
    levelLabel->setString("");
    scoreLabel->setString("0");
    
    showLevelLabel();
}

/**
 * 게임 재시작
 */
void GameView::onGameRestart() {
    
    // 블럭 리셋
    blockLayer->removeFromParent();
    blocks.clear();
    blockIndex = 0;
    
    initBlocks();
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
    
    showGameOver();
}

/**
 * 게임 모드 전환
 */
void GameView::onGameModeChanged(GameMode mode) {
    
//    feverModeBg->setVisible(mode == GameMode::FEVER);
    
    switch( mode ) {
        case GameMode::NORMAL: {
            SBAudioEngine::playBGM(SOUND_BGM_GAME);
        } break;
            
        case GameMode::FEVER: {
            SBAudioEngine::playBGM(SOUND_BGM_FEVER);
            
            // 블럭 변환
            for( auto block : blocks ) {
                block->changeBlock(RSPType::ROCK_N_ROLL);
            }
            
        } break;
    }
}

/**
 * 스코어 업데이트
 */
void GameView::updateScore() {
    
    auto saveLevelInfo = gameMgr->getLevelInfo();
    
    gameMgr->setScore(hitCount);
    scoreLabel->setString(TO_STRING(gameMgr->getScore()));
    
    // 레벨 변경된 경우, 레벨 텍스트 연출
    bool isLevelChanged = (saveLevelInfo.level != gameMgr->getLevelInfo().level);
    
    if( isLevelChanged ) {
        showLevelLabel();
    }
}

/**
 * RSP 버튼 클릭
 */
void GameView::onClickNormalButton(RSPType type) {
    
    CCASSERT(blockIndex < blocks.size(), "GameView::onClickNormalButton error: out of block index.");
    
    auto block = blocks.at(blockIndex);
    auto result = getResult(type, block->getType());
    
    man->showdown(result, type, block->getType());
    
    switch( result ) {
        case RSPResult::WIN:    this->hitBlock(block, type);      break;
        case RSPResult::LOSE:   this->misBlock(block);            break;
        case RSPResult::DRAW:   this->drawBlock(block);           break;
        default: break;
    }
}

/**
 * 피버 버튼 클릭
 */
void GameView::onClickFeverButton(int i) {
    
    CCASSERT(blockIndex < blocks.size(), "GameView::onClickFeverButton error: out of block index.");
    
    auto block = blocks.at(blockIndex);
    man->rockNroll((i == 0) ? Man::Position::LEFT : Man::Position::RIGHT);
    
    hitBlock(block, RSPType::ROCK_N_ROLL);
}

/**
 *  블럭 히트다 히트~!
 */
void GameView::hitBlock(RSPBlock *hitBlock, RSPType btnType) {
    
    // hit 카운트 증가
    ++hitCount;
    updateScore();
    
    // 시간 증가
    timeBar->increaseTimePoint(gameMgr->getConfig()->getTimeInfo().increasePointPerHit);
    
    // 히트된 블럭 연출
    runHitBlockEffect(hitBlock);
    
    // 전달된 블럭 색상 변경하여 재활용
    CCLOG("GameView::hitBlock gameMode: %d", (int)gameMgr->getGameMode());
    
    if( gameMgr->getGameMode() == GameMode::FEVER ) {
        hitBlock->changeBlock(RSPType::ROCK_N_ROLL);
    } else {
        hitBlock->changeBlock();
    }
    
    // 블럭 인덱스 증가
    ++blockIndex;
    
    if( blockIndex == blocks.size() ) {
        blockIndex = 0;
    }
    
    // 다음 블럭 타입에 따라 하단 버튼 모드를 변경
    auto nextBlock = blocks.at(blockIndex);
    
    if( nextBlock->getType() == RSPType::ROCK_N_ROLL ) {
        buttonLayer->switchButton(GameMode::FEVER);
    } else {
        buttonLayer->switchButton(GameMode::NORMAL);
    }
    
    // 정렬된 블럭 리스트 생성
    vector<RSPBlock*> sortBlocks;
    
    for( int i = blockIndex; i < blocks.size(); ++i ) {
        auto block = blocks.at(i);
        sortBlocks.push_back(block);
    }
    
    for( size_t i = 0; i < blockIndex; ++i ) {
        auto block = blocks.at(i);
        sortBlocks.push_back(block);
    }
    
    // 모든 블럭 한 칸씩 내려오기
    for( int i = 0; i < sortBlocks.size(); ++i ) {
        auto block = sortBlocks[i];
        block->stopAllActions();
        
        const float y = getBlockPosition(i).y;
        block->setPositionY(y + BLOCK_HEIGHT);
        
        auto move = MoveTo::create(BLOCK_MOVE_DURATION, Vec2(block->getPositionX(), y));
        block->runAction(move);
    }
}

/**
 *  블럭 틀렸다잉
 */
void GameView::misBlock(RSPBlock *block) {
 
    gameMgr->onGameOver();
}

/**
 * 비겼당
 */
void GameView::drawBlock(RSPBlock *block) {
    
    // 버튼 터치 지연
    buttonLayer->setButtonTouchEnabled(false);
    
    scheduleOnce([=](float dt) {
        buttonLayer->setButtonTouchEnabled(true);
    }, gameMgr->getConfig()->getTimeInfo().drawDelay, SCHEDULER_DRAW_DELAY);
}

/**
 * 히트된 블럭 연출
 */
void GameView::runHitBlockEffect(RSPBlock *hitBlock) {
    
    auto block = hitBlock->clone();
    hitBlock->getParent()->addChild(block, SBZOrder::BOTTOM);
    
    // move
    const float MOVE_DURATION = 0.15f;
    
    {
        float POS_LEFT  = 0;
        float POS_RIGHT = SB_WIN_SIZE.width;
        float posX = 0;
        
        switch( man->getManPosition() ) {
            case Man::Position::LEFT:         posX =  POS_RIGHT;    break;
            case Man::Position::RIGHT:        posX =  POS_LEFT;     break;
            default:
                CCASSERT(false, "GameView::runHitBlockEffect error: invalid man position.");
                break;
        }
        
        auto moveTo = MoveTo::create(MOVE_DURATION, Vec2(posX, block->getPositionY()));
        auto callback = CallFunc::create([=]() {
            
        });
        auto remove = RemoveSelf::create();
        block->runAction(Sequence::create(moveTo, callback, remove, nullptr));
    }
    
    // rotate
//    block->runAction(RepeatForever::create(RotateBy::create(0.1f, 90)));
    
    // fade out
    /*
    {
        auto delay = DelayTime::create(0.1f);
        auto fadeOut = FadeOut::create(MOVE_DURATION*1.05f);
        auto callback = CallFunc::create([=]() {
            
            block->stopAllActions();
        });
        auto remove = RemoveSelf::create();
        block->runAction(Sequence::create(delay, fadeOut, callback, remove, nullptr));
    }
    */
}

/**
 * 블럭 좌표 정렬
 */
void GameView::alignBlock(int i, RSPBlock *block) {
    
    block->setPosition(getBlockPosition(i));
//    block->setPosition(Vec2(SB_WIN_SIZE.width*0.5f,
//                            BLOCK_ORIGIN_POS_Y + (i*BLOCK_HEIGHT)));
}

Vec2 GameView::getBlockPosition(int i) {
    
    // x, 랜덤
    float x = 0;
    
    int ran = arc4random() % 3;
    if( ran != 0 ) {
        x = BLOCK_RANDOM_X * (ran == 1 ? 1 : -1);
    }
    
    // y, 블럭 높이 기준
    float y = (BLOCK_HEIGHT*i) + (BLOCK_HEIGHT*0.5f);
    y += (BLOCK_PADDING_Y*i); // padding    
    
    return Vec2BC(blockLayer->getContentSize(), x, y);
}

/**
 * Level 라벨 노출
 */
void GameView::showLevelLabel() {
    
    levelLabel->setVisible(true);
    
    levelLabel->stopAllActions();
    levelLabel->setString(STR_FORMAT("LEVEL %d", gameMgr->getLevelInfo().level));
    
    const Vec2 topPos    = Vec2TC(0, levelLabel->getContentSize().height);
    const Vec2 originPos = Vec2MC(0, 415);
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
 * 일시정지 팝업 노출
 */
void GameView::showPausePopup() {
    
    if( getChildByTag(Tag::POPUP_PAUSE) ) {
        // 팝업이 이미 존재함
        return;
    }
    
    gameMgr->onGamePause();
    
    auto popup = PausePopup::create();
    popup->setTag(Tag::POPUP_PAUSE);
    popup->setOnClickMenuListener([=](PausePopup::MenuType type) {
        
        switch( type ) {
            // resume
            case PausePopup::MenuType::RESUME: {
                gameMgr->onGameResume();
                popup->removeFromParent();
            } break;
                
            // main
            case PausePopup::MenuType::MAIN: {
                this->replaceMain();
                popup->removeFromParent();
                
            } break;
                
            default:
                break;
        }
    });
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
}

/**
 * 일시정지 팝업 제거
 */
void GameView::removePausePopup() {
    
    removeChildByTag(Tag::POPUP_PAUSE);
}

/**
 * 게임 오버 출력
 */
void GameView::showGameOver() {
    
    auto popup = GameOverPopup::create();
    popup->setTag(Tag::POPUP_GAME_OVER);
    popup->setOnClickMenuListener([=](GameOverPopup::MenuType type) {
        
        switch( type ) {
            // restart
            case GameOverPopup::MenuType::RESTART: {
                popup->removeFromParent();
                gameMgr->onGameRestart();
                
            } break;
                
            // home
            case GameOverPopup::MenuType::HOME: {
                popup->removeFromParent();
                this->replaceMain();
                
            } break;
                
            default:
                break;
        }
    });
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
    
    // 게임 오버 배경음 재생
    /*
     auto audioId = SBAudioEngine::playBGM(SOUND_GAME_OVER, false);
     
     CCLOG("duration: %f, %f, %f",
     SBAudioEngine::getDuration(SOUND_BGM),
     SBAudioEngine::getDuration(SOUND_GAME_OVER),
     SBAudioEngine::getDuration(audioId));
     
     // 배경음 원상복구
     SBAudioEngine::setOnFinishedListener(audioId, [=](int, const string &) {
     SBAudioEngine::playBGM(SOUND_BGM);
     });
     */
    
    /*
     scheduleOnce([=](float dt) {
     
     SBAudioEngine::playBGM(SOUND_BGM);
     
     }, SBAudioEngine::getDuration(SOUND_GAME_OVER), "GameOverSound");
     */
}

/**
 * 메인 뷰로 전환
 */
void GameView::replaceMain() {
    
    gameMgr->onExitGame();
    
    ViewManager::getInstance()->replaceMainView();
}

/**
 * 버튼 클릭
 */
void GameView::onClick(Node *sender) {
    
    switch( sender->getTag() ) {
        case Tag::BTN_PAUSE: {
            showPausePopup();
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
    
    feverModeBg = LayerColor::create(Color4B(255,0,0,255*0.2f));
    feverModeBg->setVisible(false);
    addChild(feverModeBg, -1);
}

/**
 * RSP 블럭 초기화
 */
void GameView::initBlocks() {
    
    blockLayer = Node::create();
    blockLayer->setAnchorPoint(ANCHOR_MB);
    blockLayer->setPosition(Vec2BC(0, BLOCK_ORIGIN_POS_Y));
    blockLayer->setContentSize(Size(SB_WIN_SIZE.width, BLOCK_DISPLAY_HEIGHT));
    addChild(blockLayer, -1);
    
    // 영역 테스트
    {
//        blockLayer->addChild(SBNodeUtils::createBackgroundNode(blockLayer, Color4B(255,0,0,255*0.5f)));
    }
    
    for( int i = 0; i < BLOCK_COUNT; i++ ) {
        auto block = RSPBlock::createRandomBlock();
        block->setAnchorPoint(ANCHOR_M);
        blockLayer->addChild(block);
        
        blocks.push_back(block);
        
        alignBlock(i, block);
    }
}

/**
 * RSP 버튼 초기화
 */
void GameView::initRSPButton() {
    
    buttonLayer = RSPButtonLayer::create();
    addChild(buttonLayer);
    
    gameNodes.push_back(buttonLayer);
    
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
    timeBar->setAnchorPoint(ANCHOR_MT);
    timeBar->setPosition(Vec2TC(0, -15));
    addChild(timeBar, SBZOrder::BOTTOM);
    
    gameNodes.push_back(timeBar);
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        timeBar->setPosition(Vec2TC(0, -BANNER_HEIGHT-10));
    }
}

/**
 * 라벨 초기화
 */
void GameView::initLabels() {
    
    levelLabel = Label::createWithTTF("", FONT_RETRO, 75);
    levelLabel->setVisible(false);
    levelLabel->setAnchorPoint(ANCHOR_M);
    levelLabel->setPosition(Vec2MC(0, 415));
    levelLabel->setColor(Color3B(250, 178, 11));
    levelLabel->enableOutline(Color4B(78, 22, 0, 255), 7);
    addChild(levelLabel);
    
    gameNodes.push_back(levelLabel);
    
    scoreLabel = Label::createWithTTF("0", FONT_RETRO, 75);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(0, 300));
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 7);
    addChild(scoreLabel);
    
    gameNodes.push_back(scoreLabel);
}

/**
 * 메뉴 초기화
 */
void GameView::initMenu() {
    
    auto menuLayer = SBNodeUtils::createWinSizeNode();
    menuLayer->setTag(Tag::LAYER_MENU);
    addChild(menuLayer, SBZOrder::BOTTOM);
    
    gameNodes.push_back(menuLayer);
    
    // 일시정지
    auto pauseBtn = SBButton::create(DIR_IMG_GAME + "RSP_btn_pause.png");
    pauseBtn->setTag(Tag::BTN_PAUSE);
    pauseBtn->setAnchorPoint(ANCHOR_TR);
    pauseBtn->setPosition(Vec2TR(-10, -10));
    menuLayer->addChild(pauseBtn);
    
    pauseBtn->setOnClickListener(CC_CALLBACK_1(GameView::onClick, this));
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        pauseBtn->setPosition(Vec2TR(-10, -BANNER_HEIGHT-10));
    }
}
