//
//  SBSpineHelper.cpp
//
//  Created by seongmin hwang on 2018. 6. 15..
//

#include "SBSpineHelper.hpp"

#include "../../base/SBMacros.h"

USING_NS_CC;
using namespace spine;
using namespace std;

SkeletonAnimation* SBSpineHelper::runAnimation(SBCallback completeListener,
                                            const string &jsonFile, const string &animName) {
    
    auto anim = SkeletonAnimation::createWithJsonFile(jsonFile);
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(Vec2(SB_WIN_SIZE*0.5f));
    
    if( animName != "" ) {
        runAnimation(completeListener, anim, animName);
    }
    
    return anim;
}

void SBSpineHelper::runAnimation(SBCallback completeListener, SpineAnimation *anim, const string &animName) {
    
    CCASSERT(anim != nullptr, "SBSpineHelper::runAnimation error: invalid anim.");
    CCASSERT(animName != "", "SBSpineHelper::runAnimation error: invalid anim name.");
    
    auto track = anim->setAnimation(0, animName, false);
    
    if( !completeListener ) {
        return;
    }
    
    anim->setTrackCompleteListener(track, [=](spTrackEntry *entry) {
        
        SB_SAFE_PERFORM_LISTENER(anim, completeListener);
    });
}

void SBSpineHelper::clearAnimation(SpineAnimation *anim, string clearAnimName) {
    
    CCASSERT(anim != nullptr, "SBSpineHelper::clearAnimation error: invalid anim.");
    
    anim->clearTracks();
    
    if( clearAnimName != "" ) {
        anim->setAnimation(0, clearAnimName, false);
    }
    
    anim->update(0);
}
