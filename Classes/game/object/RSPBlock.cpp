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

void RSPBlock::setBlock(RSPType type) {
    
    this->type = type;
    setTexture(UIHelper::getBlockImageFile(type));
}

void RSPBlock::changeRandomBlock() {
    
    setBlock(getRandomType());
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
