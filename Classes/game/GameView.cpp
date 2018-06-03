//
//  GameView.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameView.hpp"

#include "User.hpp"
#include "UIHelper.hpp"

#include "GameDefine.h"

#include "object/RSPBlock.hpp"
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
    
    initBlocks();
    initButtons();
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
    
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
    
    // 배너 광고 아래에 위치
    if( !User::isOwnRemoveAdsItem() ) {
        timeBar->setPosition(Vec2TC(0, -BANNER_HEIGHT-10));
        
        auto menuLayer = getChildByTag(Tag::LAYER_MENU);
        menuLayer->getChildByTag(Tag::BTN_PAUSE)->setPosition(Vec2TR(-10, -BANNER_HEIGHT-10));
    }
}

/**
 * 게임 재시작
 */
void GameView::onGameRestart() {
    
    // 블럭 초기화
    blockLayer->removeFromParent();
    blocks.clear();
    blockIndex = 0;
    
    initBlocks();
    
    // 카운트 초기화
    hitCount = 0;
    hitCountLabel->setString("");
    
    // bgm
    SBAudioEngine::playBGM(SOUND_BGM_GAME);
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
    
    showGameOver();
}

/**
 * RSP 버튼 클릭
 */
void GameView::onClickButton(RSPType type) {
    
    CCASSERT(blockIndex < blocks.size(), "GameView::onClickButton error: out of block index.");
    
    auto block = blocks.at(blockIndex);
    auto result = getResult(type, block->getType());
    
    switch( result ) {
        case RSPResult::WIN:    this->hitBlock(block, type);      break;
        case RSPResult::LOSE:   this->misBlock(block);            break;
        case RSPResult::DRAW:   this->drawBlock(block);           break;
        default: break;
    }
}

/**
 *  블럭 히트다 히트~!
 */
void GameView::hitBlock(RSPBlock *hitBlock, RSPType btnType) {
    
    // 히트된 블럭 연출
    man->hit(btnType);
    runHitBlockEffect(hitBlock);
    
    // 전달된 블럭 색상 변경하여 재활용
    hitBlock->changeBlock();
    
    // hit 카운트 증가
    ++hitCount;
    hitCountLabel->setString(TO_STRING(hitCount));
    
    // 시간 증가
    timeBar->increase(INCREASE_TIME_PER_HIT);
    
    // 블럭 인덱스 증가
    ++blockIndex;
    
    if( blockIndex == blocks.size() ) {
        blockIndex = 0;
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

    man->showdown();
//    man->draw();
    
    auto setButtonTouchEnabled = [=](bool enabled) {
        for( auto btn : rspButtons ) {
            btn->setTouchEnabled(enabled);
        }
    };
    
    // 버튼 터치 지연
    setButtonTouchEnabled(false);
    
    scheduleOnce([=](float dt) {
        setButtonTouchEnabled(true);
    }, DRAW_DELAY, SCHEDULER_DRAW_DELAY);
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
    float y = (BLOCK_HEIGHT*i) + (BLOCK_HEIGHT*0.5f);
    y += (BLOCK_PADDING_Y*i); // padding
    
    return Vec2BC(blockLayer->getContentSize(), 0, y);
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
    addChild(popup, SBZOrder::TOP);
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
    addChild(popup, SBZOrder::TOP);
    
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
 * RSP 블럭 초기화
 */
void GameView::initBlocks() {
    
    blockLayer = Node::create();
    blockLayer->setAnchorPoint(ANCHOR_MB);
    blockLayer->setPosition(Vec2BC(0, BLOCK_ORIGIN_POS_Y));
    blockLayer->setContentSize(Size(SB_WIN_SIZE.width, BLOCK_DISPLAY_HEIGHT));
    addChild(blockLayer);
    
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
void GameView::initButtons() {
    
    const float POS_Y = 5;
    
    RSPType types[] = {
        RSPType::SCISSORS,
        RSPType::ROCK,
        RSPType::PAPER,
    };
    
    SBUIInfo uiInfos[] = {
        SBUIInfo(ANCHOR_BL, Vec2BL(20,   POS_Y),    ""),
        SBUIInfo(ANCHOR_MB, Vec2BC(0,    POS_Y),    ""),
        SBUIInfo(ANCHOR_BR, Vec2BR(-20,  POS_Y),    ""),
    };
    
    for( int i = 0; i < sizeof(types) / sizeof(RSPType); ++i ) {
        auto type = types[i];
        
        auto btn = RSPButton::create(type);
        uiInfos[i].apply(btn);
        addChild(btn);
        
        rspButtons.push_back(btn);
        gameNodes.push_back(btn);
        
        btn->setOnClickListener([=](Node*) {
            this->onClickButton(type);
        });
    }
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
    
    /*
    leveLabel = Label::createWithTTF("LEVEL 1", FONT_RETRO, 80);
    leveLabel->setAnchorPoint(ANCHOR_M);
    leveLabel->setPosition(Vec2MC(0, 500));
    leveLabel->setColor(Color3B(244, 206, 66));
    leveLabel->enableOutline(Color4B(244, 178, 65, 255), 8);
    addChild(leveLabel);
    */
    
    hitCountLabel = Label::createWithTTF("", FONT_RETRO, 75);
    hitCountLabel->setAnchorPoint(ANCHOR_M);
    hitCountLabel->setPosition(Vec2MC(0, 260));
    hitCountLabel->setColor(Color3B::WHITE);
    hitCountLabel->enableOutline(Color4B::BLACK, 7);
    addChild(hitCountLabel);
    
    gameNodes.push_back(hitCountLabel);
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
