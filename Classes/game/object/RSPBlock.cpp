//
//  RSPBlock.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 15..
//

#include "RSPBlock.hpp"

#include "UIHelper.hpp"
#include "../GameDefine.h"

USING_NS_CC;
using namespace std;

static const float HIT_BLOCK_MOVE_DURATION = 0.13f;

/**
 * create
 */
RSPBlock* RSPBlock::create(RSPType type) {
    
    auto block = new RSPBlock();
    
    if( block && block->init(type) ) {
        block->autorelease();
        return block;
    }
    
    CC_SAFE_DELETE(block);
    return nullptr;
}

RSPBlock* RSPBlock::createRandomBlock() {
    
    return create(getRandomType());
}

RSPBlock::RSPBlock() :
type(RSPType::NONE),
idx(INVALID_INDEX),
prevBlock(nullptr) {
}

RSPBlock::~RSPBlock() {
}

/**
 * init
 */
bool RSPBlock::init(RSPType type) {
    
    if( !Sprite::init() ) {
        return false;
    }
    
    this->type = type;
    
    if( type != RSPType::NONE ) {
        setBlock(type);
    }
    
    return true;
}

/**
 * 블럭 설정
 */
void RSPBlock::setBlock(RSPType type) {
    
    this->type = type;
    
    // 락앤롤만 flip
    if( type == RSPType::ROCK_N_ROLL ) {
        bool flippedX = (arc4random() % 2 == 0);
        setTexture(getBlockImageFile(type, flippedX));
    } else {
        setTexture(getBlockImageFile(type));
    }
    
    setPositionX(getBlockPositionX(type, prevBlock));
}

/**
 * 인덱스 설정
 */
void RSPBlock::setIndex(int i) {
    
    this->idx = i;
    
    if( i != INVALID_INDEX ) {
        setLocalZOrder(i);
        setPositionY(getBlockPositionY(i));
    }
}

/**
 * 랜덤한 블럭으로 변경
 */
void RSPBlock::changeRandomBlock() {
    
    setBlock(getRandomType());
}

/**
 * 아래로 1칸 이동
 */
void RSPBlock::downWithAction() {
    
    CCASSERT(idx > 0, "RSPBlock::downWithAction error.");
    
    int oldIdx = idx;
    int newIdx = idx-1;
    
    stopAllActions();
    setIndex(newIdx);
    
    setPositionY(getBlockPositionY(oldIdx));
    runMoveAction(BLOCK_MOVE_DURATION, Vec2(getPositionX(), getBlockPositionY(newIdx)));
}

/**
 * 히트 연출
 */
void RSPBlock::runHitAction(bool isManOnLeft) {
    
    // create effect block
    auto effectBlock = clone();
    effectBlock->setPositionY(getBlockPositionY(0));
    getParent()->addChild(effectBlock, -1);
    
    // action
    float POS_LEFT  = -BLOCK_WIDTH * 0.5f;
    float POS_RIGHT = SB_WIN_SIZE.width + (BLOCK_WIDTH * 0.5f);
    float posX = isManOnLeft ? POS_RIGHT : POS_LEFT;
    
    // move
    auto move = MoveTo::create(HIT_BLOCK_MOVE_DURATION, Vec2(posX, effectBlock->getPositionY()));
    auto callback = CallFunc::create([=]() {
    });
    auto remove = RemoveSelf::create();
    effectBlock->runAction(Sequence::create(move, callback, remove, nullptr));
}

/**
 * 비김 연출 (또잉~)
 */
void RSPBlock::runDrawAction(bool isManOnLeft, DrawAnimEventListener eventListener) {
    
    // 애니메이션 진행되는 동안 원본 블럭은 hide
    setVisible(false);
    
    string animName = isManOnLeft ? ANIM_NAME_DRAW_LEFT[type] : ANIM_NAME_DRAW_RIGHT[type];
    auto anim = SBSpineHelper::runAnimation([=]() {
        this->setVisible(true);
    }, ANIM_DRAW, animName, true);
    
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(getPosition());
    getParent()->addChild(anim, getLocalZOrder());
    
    // 이벤트 리스너
    anim->setEventListener([=](spTrackEntry *entry, spEvent *event) {
        
        string eventName = event->data->name;
        
        if( !SBStringUtils::isContains(eventName, ANIM_EVENT_MAN_MOVE) ) {
            return;
        }
        
        int i = SBStringUtils::toNumber<int>(eventName.substr(eventName.length()-1));
        eventListener(i);
    });
}

void RSPBlock::runMoveAction(float duration, const Vec2 &pos) {
    
    moveTargetPosition = pos;
    
    auto move = MoveTo::create(duration, pos);
    move->setTag(ACTION_TAG_MOVE);
    runAction(move);
}

void RSPBlock::stopMoveAction() {
    
    if( getActionByTag(ACTION_TAG_MOVE) ) {
        stopActionByTag(ACTION_TAG_MOVE);
        setPosition(moveTargetPosition);
    }
}

/**
 * 블럭 좌표 반환
 */
Vec2 RSPBlock::getBlockPosition(RSPType type, int i, RSPBlock *prevBlock) {
    
    return Vec2(getBlockPositionX(type, prevBlock), getBlockPositionY(i));
}

float RSPBlock::getBlockPositionX(RSPType type, RSPBlock *prevBlock) {
    
    if( type == RSPType::ROCK_N_ROLL ) {
        return BLOCK_POS_CENTER;
    }
    
    // 이전 블럭이 없거나 락앤롤 타입이면 랜덤 좌표
    if( !prevBlock || prevBlock->getType() == RSPType::ROCK_N_ROLL ) {
        return (arc4random() % 2 == 0) ? BLOCK_POS_LEFT : BLOCK_POS_RIGHT;
    }
    
    // 이전 블럭과 같은 타입
    if( type == prevBlock->getType() ) {
        return prevBlock->getPositionX();
    }
    
    // 이전 블럭과 다른 타입
    return prevBlock->getPositionX() == BLOCK_POS_LEFT ? BLOCK_POS_RIGHT : BLOCK_POS_LEFT;
}

float RSPBlock::getBlockPositionY(int i) {
    
    if( i == INVALID_INDEX ) {
        return 0;
    }
    
    float y = (BLOCK_HEIGHT*i) + (BLOCK_HEIGHT*0.5f);
    y += (BLOCK_PADDING_Y*i); // padding
    
    return y;
}

string RSPBlock::getBlockImageFile(RSPType type, bool flippedX) {
    
    switch( type ) {
        case RSPType::ROCK:          return DIR_IMG_GAME + "RSP_block_rock.png";
        case RSPType::SCISSORS:      return DIR_IMG_GAME + "RSP_block_scissors.png";
        case RSPType::PAPER:         return DIR_IMG_GAME + "RSP_block_paper.png";
        case RSPType::ROCK_N_ROLL:
            if( flippedX ) {
                return DIR_IMG_GAME + "RSP_block_fever.png";
            } else {
                return DIR_IMG_GAME + "RSP_block_fever2.png";
            }
        default:
            CCASSERT(false, "UIHelper::getBlockImageFile error: invalid rsp type.");
            break;
    }
    
    return "";
}

RSPType RSPBlock::getRandomType() {
    
    int ran = arc4random() % 3;
    return (RSPType)(ran + (int)RSPType::ROCK);
}

RSPBlock* RSPBlock::clone() {
    
    auto block = RSPBlock::create(type);
    block->setAnchorPoint(getAnchorPoint());
    block->setPosition(getPosition());
    
    return block;
}

string RSPBlock::toString() {
    return STR_FORMAT("RSPBlock[%d]: %s", idx, typeToString(type).c_str());
}
