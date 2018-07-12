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
    
    auto getAnimPNG = [](string anim) -> string {
        return SBStringUtils::replaceAll(anim, ANIM_EXT, ".png");
    };
    
    // add image
    {
        string files[] = {
            ANIM_CLOUD,
        };
        
        for( string file : files ) {
            textureCache->addImage(getAnimPNG(file));
        }
    }
    
    // add image async
    {
        string files[] = {
            ANIM_CLOUD_DARK,
            ANIM_FEVER_MODE_BG,
            ANIM_FEVER_MODE_FIRE,
            ANIM_DRAW,
            ANIM_DIE,
            ANIM_CONTINUE,
        };
        
        for( string file : files ) {
            textureCache->addImageAsync(getAnimPNG(file), nullptr);
        }
        
        // ...2.png
        {
            string files[] = {
                ANIM_CLOUD_DARK,
                ANIM_DIE,
                ANIM_CONTINUE,
            };
            
            for( string file : files ) {
                const string secondFile = SBStringUtils::replaceAll(getAnimPNG(file), ".png", "2.png");
                textureCache->addImageAsync(getAnimPNG(secondFile), nullptr);
            }
        }
    }
    
    // sound
    SBAudioEngine::preload(SOUND_BGM_MAIN);
    SBAudioEngine::preload(SOUND_BGM_GAME);
    SBAudioEngine::preload(SOUND_BGM_FEVER);
    SBAudioEngine::preload(SOUND_PUNCH);
    SBAudioEngine::preload(SOUND_GAME_OVER);
}
