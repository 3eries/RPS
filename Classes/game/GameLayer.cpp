//
//  GameLayer.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameLayer.hpp"

#include "UIHelper.hpp"
#include "GameValue.hpp"

#include "ui/RSPBlock.hpp"
#include "ui/RSPButton.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

GameLayer::GameLayer() :
onGameOverListener(nullptr),
blockLayer(nullptr),
blockIndex(0),
hitCount(0) {
}

GameLayer::~GameLayer() {
    
    blocks.clear();
}

bool GameLayer::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBlocks();
    initButtons();
    initLabels();
    
    return true;
}

void GameLayer::onEnter() {
    
    Node::onEnter();
    
    // 연출
    const float POS_Y = blockLayer->getPositionY();
    blockLayer->setPositionY(POS_Y + blockLayer->getContentSize().height);
    
    auto move = MoveTo::create(0.3f, Vec2(blockLayer->getPositionX(), POS_Y));
    blockLayer->runAction(move);
}

void GameLayer::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void GameLayer::onExit() {
    
    Node::onExit();
}

/**
 * RSP 버튼 클릭
 */
void GameLayer::onClickButton(RSPType type) {
    
    CCASSERT(blockIndex < blocks.size(), "GameScene::onClickButton error: out of block index.");
    
    auto block = blocks.at(blockIndex);
    bool isHit = false;
    
    switch( block->getType() ) {
        case RSPType::ROCK:      isHit = (type == RSPType::PAPER);      break;
        case RSPType::SCISSORS:  isHit = (type == RSPType::ROCK);       break;
        case RSPType::PAPER:     isHit = (type == RSPType::SCISSORS);   break;
        default:
            CCASSERT(false, "GameLayer::onClickButton error: invalid block type.");
            break;
    }
    
    // 정답
    if( isHit ) {
        hitBlock(block);
    }
    // 오답
    else {
        misBlock(block);
    }
}

/**
 *  블럭 히트다 히트~!
 */
void GameLayer::hitBlock(RSPBlock *hitBlock) {
    
    // 히트된 블럭 연출
    runHitBlockEffect(hitBlock);
    
    // 전달된 블럭 색상 변경하여 재활용
    hitBlock->changeBlock();
    
    // hit 카운트 증가
    ++hitCount;
    hitCountLabel->setString(TO_STRING(hitCount));
    
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
void GameLayer::misBlock(RSPBlock *block) {
 
    onGameOverListener();
}

/**
 * 히트된 블럭 연출
 */
void GameLayer::runHitBlockEffect(RSPBlock *hitBlock) {
    
    SBAudioEngine::play2d(SOUND_PUNCH);
    
    auto block = hitBlock->clone();
    hitBlock->getParent()->addChild(block, SBZOrder::BOTTOM);
    
    // move
    const float MOVE_DURATION = 0.15f;
    
    {
        const float MOVE_POS_Y = BLOCK_DISPLAY_HEIGHT*0.5f;
        
        Vec2 MOVE_LEFT_POS  = Vec2(0,                   MOVE_POS_Y);
        Vec2 MOVE_RIGHT_POS = Vec2(SB_WIN_SIZE.width,   MOVE_POS_Y);
        Vec2 movePos;
        
        switch( block->getType() ) {
            case RSPType::ROCK:         movePos = MOVE_LEFT_POS;    break;
            case RSPType::SCISSORS:     movePos = MOVE_RIGHT_POS;   break;
            case RSPType::PAPER: {
                int ran = arc4random() % 2;
                movePos = (ran == 0) ? MOVE_LEFT_POS : MOVE_RIGHT_POS;
            } break;
                
            default:
                CCASSERT(false, "GameLayer::hitBlock error: invalid block type.");
                break;
        }
        
        auto moveTo = MoveTo::create(MOVE_DURATION, movePos);
        auto callback = CallFunc::create([=]() {
            
        });
        block->runAction(Sequence::create(moveTo, callback, nullptr));
    }
    
    // rotate
    block->runAction(RepeatForever::create(RotateBy::create(0.1f, 90)));
    
    // fade out
    {
        auto delay = DelayTime::create(0.1f);
        auto fadeOut = FadeOut::create(MOVE_DURATION*1.05f);
        auto callback = CallFunc::create([=]() {
            
            block->stopAllActions();
        });
        auto remove = RemoveSelf::create();
        block->runAction(Sequence::create(delay, fadeOut, callback, remove, nullptr));
    }
}

/**
 * 블럭 좌표 정렬
 */
void GameLayer::alignBlock(int i, RSPBlock *block) {
    
    block->setPosition(getBlockPosition(i));
//    block->setPosition(Vec2(SB_WIN_SIZE.width*0.5f,
//                            BLOCK_ORIGIN_POS_Y + (i*BLOCK_HEIGHT)));
}

Vec2 GameLayer::getBlockPosition(int i) {
    return Vec2BC(blockLayer->getContentSize(), 0, (BLOCK_HEIGHT*i) + (BLOCK_HEIGHT*0.5f));
}

/**
 * RSP 블럭 초기화
 */
void GameLayer::initBlocks() {
    
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
void GameLayer::initButtons() {
    
    const float POS_Y = 15;
    
    RSPType types[] = {
        RSPType::ROCK,
        RSPType::SCISSORS,
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
        
        btn->setOnClickListener([=](Node*) {
            this->onClickButton(type);
        });
    }
}

/**
 * 라벨 초기화
 */
void GameLayer::initLabels() {
    
    hitCountLabel = Label::createWithTTF("", FONT_RETRO, 40);
    hitCountLabel->setAnchorPoint(ANCHOR_M);
    hitCountLabel->setPosition(Vec2MC(0, 420));
    hitCountLabel->setColor(Color3B::WHITE);
    hitCountLabel->enableOutline(Color4B::BLACK, 3);
    hitCountLabel->setScale(2);
    addChild(hitCountLabel);
}

