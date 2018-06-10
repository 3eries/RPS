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
    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->addImageAsync(DIR_ANIM + "RSP_cloud.png", nullptr);
    
    // sound
    SBAudioEngine::preload(SOUND_BGM_MAIN);
    SBAudioEngine::preload(SOUND_BGM_GAME);
    SBAudioEngine::preload(SOUND_BGM_FEVER);
    SBAudioEngine::preload(SOUND_PUNCH);
    SBAudioEngine::preload(SOUND_GAME_OVER);
}
