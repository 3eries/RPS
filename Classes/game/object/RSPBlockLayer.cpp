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
blockIndex(0),
onFirstBlockChangedListener(nullptr) {
    
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
    setContentSize(BLOCK_LAYER_SIZE);
    
    initBlocks();
    
    gameMgr->addListener(this);
    
    return true;
}

void RSPBlockLayer::onEnter() {
    
    Node::onEnter();
}

void RSPBlockLayer::onExit() {
    
    gameMgr->removeListener(this);
    
    Node::onExit();
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
    
    resetBlocks();
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
            // 블럭 전환
            for( auto block : blocks ) {
                block->setBlock(RSPType::ROCK_N_ROLL);
            }
            
        } break;
    }
}

/**
 * 블럭 리셋
 */
void RSPBlockLayer::resetBlocks() {
    
    for( auto block : blocks ) {
        block->removeFromParent();
    }
    
    blocks.clear();
    blockIndex = 0;
    
    initBlocks();
    
    if( onFirstBlockChangedListener ) {
        onFirstBlockChangedListener(getFirstBlock());
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
 * 정렬된 블럭 목록
 */
std::vector<RSPBlock*> RSPBlockLayer::getSortedBlocks() {
    
    vector<RSPBlock*> sortBlocks;
    
    for( int i = blockIndex; i < blocks.size(); ++i ) {
        auto block = blocks.at(i);
        sortBlocks.push_back(block);
    }
    
    for( size_t i = 0; i < blockIndex; ++i ) {
        auto block = blocks.at(i);
        sortBlocks.push_back(block);
    }
    
    return sortBlocks;
}

/**
 *  블럭 히트다 히트~!
 */
void RSPBlockLayer::hitBlock(RSPBlock *hitBlock, RSPType btnType, bool isManOnLeft) {
    
    const RSPType hitBlockType = hitBlock->getType();
    
    // 히트 블럭 연출
    hitBlock->runHitAction(isManOnLeft);
    
    // 히트 블럭 타입 변경하여 재활용
    hitBlock->setBlock(getBlockType(blockIndex));
    
    // 블럭 인덱스 증가
    ++blockIndex;
    
    if( blockIndex == blocks.size() ) {
        blockIndex = 0;
    }
    
    // 마지막 피버 블럭인지 체크
    if( hitBlockType == RSPType::ROCK_N_ROLL && blocks[blockIndex]->getType() != RSPType::ROCK_N_ROLL ) {
        gameMgr->onLastFeverBlockHit();
    }
    
    // 블럭 한 칸씩 내려오기
    auto sortBlocks = getSortedBlocks();
    
    for( int i = 0; i < BLOCK_DISPLAY_COUNT; ++i ) {
        auto block = sortBlocks[i];
        block->setVisible(true);
        block->downWithAction();
    }
    
    // 화면 밖 블럭 hide
    for( int i = BLOCK_DISPLAY_COUNT; i < sortBlocks.size(); ++i ) {
        auto block = sortBlocks[i];
        block->setIndex(i);
        block->setVisible(false);
    }
    
    onFirstBlockChangedListener(getFirstBlock());
    
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
void RSPBlockLayer::drawBlock(RSPBlock *block, bool isManOnLeft, DrawAnimEventListener eventListener) {
    
    auto sortBlocks = getSortedBlocks();

    for( int i = 0; i < BLOCK_DISPLAY_COUNT; ++i ) {
        auto block = sortBlocks[i];
        block->stopAllActions();
        block->refreshPosition();
    }
    
    block->runDrawAction(isManOnLeft, eventListener);
}

/**
 * 해당 인덱스의 블럭 타입 반환
 */
RSPType RSPBlockLayer::getBlockType(int i) {
    
    // 피버 모드, 락앤롤
    if( gameMgr->getGameMode() == GameMode::FEVER ) {
        return RSPType::ROCK_N_ROLL;
    }
    
    // 노멀 모드
    size_t prev = (i == 0) ? blocks.size()-1 : i-1;
    auto   prevType = blocks[prev]->getType();
    
    if( prevType == RSPType::ROCK_N_ROLL ) {
        // 이전 타입이 락앤롤
        return RSPBlock::getRandomType();
    }
    
    // 블럭 연속될 확률 체크
    const int continuation = gameMgr->getLevelInfo().continuation;
    int ran = random<int>(1, 100);
    
    if( ran <= continuation ) {
        // 이전과 같은 타입
        return prevType;
    }
     
    // 랜덤 타입
    return RSPBlock::getRandomType();
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
        auto block = RSPBlock::create(RSPType::NONE);
        block->setIndex(i);
        block->setAnchorPoint(ANCHOR_M);
        addChild(block);
        
        blocks.push_back(block);
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
    
    vector<RSPBlock*> sortBlocks = getSortedBlocks();
    
    for( int i = (int)sortBlocks.size()-1; i >= 0; --i ) {
        auto block = sortBlocks[i];
        str += STR_FORMAT("\t%s, index of blocks: %d\n", block->toString().c_str(), (int)SBCollection::getIndex(blocks, block));
    }
    
    str += "]";
    return str;
}
