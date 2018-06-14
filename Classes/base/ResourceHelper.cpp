//
//  ResourceHelper.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ResourceHelper.hpp"

#include "superbomb.h"
#include "RSP.h"

USING_NS_CC;
using namespace std;

void ResourceHelper::preload() {
    
    // image
    auto getAnimPNG = [](string anim) -> string {
        return SBStringUtils::replaceAll(anim, ANIM_EXT, ".png");
    };
    
    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->addImage(getAnimPNG(ANIM_CLOUD));
    textureCache->addImageAsync(getAnimPNG(ANIM_FEVER_MODE_BG), nullptr);
    textureCache->addImageAsync(getAnimPNG(ANIM_FEVER_MODE_FIRE), nullptr);
    textureCache->addImageAsync(getAnimPNG(ANIM_DRAW), nullptr);
    textureCache->addImageAsync(getAnimPNG(ANIM_DIE), [=](Texture2D *tex) {
        CCLOG("add image completed : %d", tex != nullptr);
    });
    
    // sound
    SBAudioEngine::preload(SOUND_BGM_MAIN);
    SBAudioEngine::preload(SOUND_BGM_GAME);
    SBAudioEngine::preload(SOUND_BGM_FEVER);
    SBAudioEngine::preload(SOUND_PUNCH);
    SBAudioEngine::preload(SOUND_GAME_OVER);
}
