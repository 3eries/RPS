//
//  RSPBlock.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 15..
//

#include "RSPBlock.hpp"

#include "UIHelper.hpp"

USING_NS_CC;
using namespace std;

RSPType RSPBlock::getRandomType() {
    
    int ran = arc4random() % 3;
    return (RSPType)(ran + (int)RSPType::ROCK);
}

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
type(RSPType::NONE) {
}

RSPBlock::~RSPBlock() {
}

bool RSPBlock::init(RSPType type) {
    
    if( !Sprite::init() ) {
        return false;
    }
    
    this->type = type;
    
    if( type != RSPType::NONE ) {
        setTexture(UIHelper::getBlockImageFile(type));
    }
    
    return true;
}

/**
 * 블럭 설정
 */
void RSPBlock::setBlock(RSPType type) {
    
    this->type = type;
    setTexture(UIHelper::getBlockImageFile(type));
}

/**
 * 랜덤한 블럭으로 변경
 */
void RSPBlock::changeRandomBlock() {
    
    setBlock(getRandomType());
}

/**
 * 비김 연출 재생 (또잉~)
 */
void RSPBlock::runDrawAnimation(bool isLeft, function<void(int)> eventListener) {
    
    // 애니메이션 진행되는 동안 원본 블럭은 hide
    setVisible(false);
    
    string animName = isLeft ? ANIM_NAME_DRAW_LEFT[type] : ANIM_NAME_DRAW_RIGHT[type];
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

bool RSPBlock::isEqualBlock(RSPType type) {
    
    return this->type == type;
}

RSPBlock* RSPBlock::clone() {
    
    auto block = RSPBlock::create(type);
    block->setAnchorPoint(getAnchorPoint());
    block->setPosition(getPosition());
    
    return block;
}

string RSPBlock::toString() {
    return STR_FORMAT("RSPBlock [%s]", typeToString(type).c_str());
}
