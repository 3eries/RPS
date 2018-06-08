//
//  RSPBlockLayer.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 7..
//

#include "RSPBlockLayer.hpp"

#include "GameConfiguration.hpp"
#include "UIHelper.hpp"

#include "../GameDefine.h"
#include "RSPBlock.hpp"

USING_NS_CC;
using namespace std;

RSPBlockLayer::RSPBlockLayer() :
gameMgr(GameManager::getInstance()),
blockIndex(0) {
    
}

RSPBlockLayer::~RSPBlockLayer() {
    
    blocks.clear();
}

bool RSPBlockLayer::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_MB);
    setPosition(Vec2BC(0, BLOCK_ORIGIN_POS_Y));
    setContentSize(Size(SB_WIN_SIZE.width, BLOCK_DISPLAY_HEIGHT));
    
    initBlocks();
    
    gameMgr->addListener(this);
    ViewManager::getInstance()->addListener(this);
    
    return true;
}

void RSPBlockLayer::onEnter() {
    
    Node::onEnter();
}

void RSPBlockLayer::onExit() {
    
    ViewManager::getInstance()->removeListener(this);
    gameMgr->removeListener(this);
    
    Node::onExit();
}

/**
 * 뷰 타입 변경
 */
void RSPBlockLayer::onViewChanged(ViewType viewType) {
    
    switch( viewType ) {
        case ViewType::MAIN: {
        } break;
            
        case ViewType::GAME: {
        } break;
            
        default:
            break;
    }
}

/**
 * 게임 시작
 */
void RSPBlockLayer::onGameStart() {
}

/**
 * 게임 재시작
 */
void RSPBlockLayer::onGameRestart() {
    
    // 블럭 리셋
    for( auto block : blocks ) {
        block->removeFromParent();
    }
    
    blocks.clear();
    blockIndex = 0;
    
    initBlocks();
}

/**
 * 게임 오버
 */
void RSPBlockLayer::onGameOver() {
}

/**
 * 게임 모드 전환
 */
void RSPBlockLayer::onGameModeChanged(GameMode mode) {
    
    switch( mode ) {
        case GameMode::NORMAL: {
        } break;
            
        case GameMode::FEVER: {
            // 블럭 변환
            for( auto block : blocks ) {
                block->setBlock(RSPType::ROCK_N_ROLL);
            }
            
        } break;
    }
}

/**
 * 첫번째 블럭 반환, 화면 상에 제일 아래에 위치한 블럭
 */
RSPBlock* RSPBlockLayer::getFirstBlock() {
    
    CCASSERT(blockIndex < blocks.size(), "RSPBlockLayer::getFirstBlock error: out of block index.");
    
    return blocks[blockIndex];
}

/**
 *  블럭 히트다 히트~!
 */
void RSPBlockLayer::hitBlock(RSPBlock *hitBlock, RSPType btnType,
                             Man::Position manPosition) {
    
    // 히트된 블럭 연출
    runHitBlockEffect(hitBlock, manPosition);
    
    // 전달된 블럭 타입 변경하여 재활용
    hitBlock->setBlock(getBlockType(blockIndex));
    
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
    
    CCLOG("RSPBlockLayer::hitBlock: %s", toString().c_str());
}

/**
 *  블럭 틀렸다잉
 */
void RSPBlockLayer::misBlock(RSPBlock *block) {
}

/**
 * 비겼당
 */
void RSPBlockLayer::drawBlock(RSPBlock *block) {
}

/**
 * 히트된 블럭 연출
 */
void RSPBlockLayer::runHitBlockEffect(RSPBlock *hitBlock, Man::Position manPosition) {
    
    auto block = hitBlock->clone();
    hitBlock->getParent()->addChild(block, SBZOrder::BOTTOM);
    
    // move
    const float MOVE_DURATION = 0.15f;
    
    {
        float POS_LEFT  = 0;
        float POS_RIGHT = SB_WIN_SIZE.width;
        float posX = 0;
        
        switch( manPosition ) {
            case Man::Position::LEFT:         posX =  POS_RIGHT;    break;
            case Man::Position::RIGHT:        posX =  POS_LEFT;     break;
            default:
                CCASSERT(false, "RSPBlockLayer::runHitBlockEffect error: invalid man position.");
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
 * 해당 인덱스의 블럭 타입 반환
 */
RSPType RSPBlockLayer::getBlockType(int i) {
    
    if( gameMgr->getGameMode() == GameMode::FEVER ) {
        return RSPType::ROCK_N_ROLL;
    }
    
    // 블럭 연속될 확률 체크
    int continuation = gameMgr->getLevelInfo().continuation;
    int ran = (arc4random() % 100) + 1;
    
    if( ran <= continuation ) {
        // 이전과 동일한 타입
        int prev = (i == 0) ? blocks.size()-1 : i-1;
        return blocks[prev]->getType();
    }
    
    // 랜덤 타입
    return RSPBlock::getRandomType();
}

/**
 * 해당 인덱스의 블럭 좌표 정렬
 */
void RSPBlockLayer::alignBlock(int i, RSPBlock *block) {
    
    block->setPosition(getBlockPosition(i));
}

/**
 * 해당 인덱스의 블럭 좌표 반환
 */
Vec2 RSPBlockLayer::getBlockPosition(int i) {
    
    // x, 랜덤
    float x = 0;
    
    int ran = arc4random() % 3;
    if( ran != 0 ) {
        x = BLOCK_RANDOM_X * (ran == 1 ? 1 : -1);
    }
    
    // y, 블럭 높이 기준
    float y = (BLOCK_HEIGHT*i) + (BLOCK_HEIGHT*0.5f);
    y += (BLOCK_PADDING_Y*i); // padding
    
    return Vec2BC(getContentSize(), x, y);
}

/**
 * 블럭 초기화
 */
void RSPBlockLayer::initBlocks() {
    
    // 영역 테스트
    {
        // addChild(SBNodeUtils::createBackgroundNode(blockLayer, Color4B(255,0,0,255*0.5f)));
    }
    
    // 블랙 생성
    for( int i = 0; i < BLOCK_COUNT; i++ ) {
        //        auto block = RSPBlock::createRandomBlock();
        auto block = RSPBlock::create(RSPType::NONE);
        block->setAnchorPoint(ANCHOR_M);
        addChild(block);
        
        blocks.push_back(block);
        
        alignBlock(i, block);
    }
    
    // 블럭 타입 설정
    blocks[0]->changeRandomBlock();
    
    for( int i = 1; i < BLOCK_COUNT; i++ ) {
        auto block = blocks[i];
        block->setBlock(getBlockType(i));
    }
    
    CCLOG("RSPBlockLayer::initBlocks: %s", toString().c_str());
}

string RSPBlockLayer::toString() {
    
    string str = "RSPBlockLayer [\n";
    
    vector<RSPBlock*> sortBlocks;
    
    for( int i = blockIndex; i < blocks.size(); ++i ) {
        auto block = blocks.at(i);
        sortBlocks.push_back(block);
    }
    
    for( size_t i = 0; i < blockIndex; ++i ) {
        auto block = blocks.at(i);
        sortBlocks.push_back(block);
    }
    
    // for( int i = 0; i < blocks.size(); ++i ) {
    for( int i = 0; i < sortBlocks.size(); ++i ) {
        str += STR_FORMAT("%d: %s\n", i, blocks[i]->toString().c_str());
    }
    
    str += "]";
    return str;
}