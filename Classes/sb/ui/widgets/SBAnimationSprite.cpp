//
//  SBAnimationSprite.cpp
//  Snow Bros-mobile
//
//  Created by seongmin hwang on 2018. 3. 7..
//

#include "SBAnimationSprite.hpp"

#include "../../utils/SBCollection.hpp"

USING_NS_CC;
using namespace std;

SBAnimationSprite* SBAnimationSprite::create(Animation *anim, int loops) {
    
    auto spr = new SBAnimationSprite();
    
    if( spr && spr->init(anim, loops) ) {
        spr->autorelease();
        return spr;
    }
    
    CC_SAFE_DELETE(spr);
    return nullptr;
}

SBAnimationSprite* SBAnimationSprite::create(vector<string> animFiles,
                                             float delayPerUnit, int loops) {
    
    auto anim = Animation::create();
    anim->setDelayPerUnit(delayPerUnit);
    
    for( int i = 0; i < animFiles.size(); ++i ) {
        anim->addSpriteFrameWithFile(animFiles[i]);
    }
    
    return create(anim, loops);
}

SBAnimationSprite::SBAnimationSprite() :
anim(nullptr),
loops(0),
runningAnimation(false),
action(nullptr) {
    
}

SBAnimationSprite::~SBAnimationSprite() {
    
    CC_SAFE_RELEASE_NULL(anim);
}

bool SBAnimationSprite::init(Animation *anim, int loops) {
    
    CCASSERT(anim != nullptr, "SBAnimationSprite::init error: invalid animation.");
    CCASSERT(loops >= LOOP_FOREVER, "SBAnimationSprite::init error: invalid loops.");
    
    auto frame = anim->getFrames().at(0)->getSpriteFrame();
    
    if( !Sprite::initWithSpriteFrame(frame) ) {
        return false;
    }
    
    this->anim = anim->clone();
    this->anim->retain();
    
    this->loops = loops;
    
    if( loops == LOOP_FOREVER ) {
        this->anim->setLoops(1);
    } else {
        this->anim->setLoops((unsigned int)loops);
    }
    
    return true;
}

void SBAnimationSprite::setSpriteFrame(size_t i) {
    
    CCASSERT(anim != nullptr, "SBAnimationSprite::setDisplayFrame error: invalid animation.");
    CCASSERT(i < anim->getFrames().size(), "SBAnimationSprite::setDisplayFrame error: invalid index.");
    
    auto frame = anim->getFrames().at(i)->getSpriteFrame();
    Sprite::setSpriteFrame(frame);
}

/**
 * 애니메이션 재생
 */
void SBAnimationSprite::runAnimation(SBCallbackNode onCompletedListener) {
    
    stopAnimation();
    
    runningAnimation = true;
    
    auto animate = Animate::create(anim);
    
    if( loops == LOOP_FOREVER ) {
        action = RepeatForever::create(animate);
    } else {
        auto callFunc = CallFunc::create([=]() {
            
            this->runningAnimation = false;
            SB_SAFE_PERFORM_LISTENER_N(this, onCompletedListener);
        });
        
        action = Sequence::create(animate, callFunc, nullptr);
    }
    
    runAction(action);
}

/**
 * 애니메이션 정지
 */
void SBAnimationSprite::stopAnimation() {
    
    stopAction(action);
    action = nullptr;
    
    setSpriteFrame(0);
    
    runningAnimation = false;
}

