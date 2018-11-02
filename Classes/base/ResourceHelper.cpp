//
//  ResourceHelper.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ResourceHelper.hpp"

#include "superbomb.h"
#include "Define.h"
#include "CharacterManager.hpp"

USING_NS_CC;
using namespace std;

void ResourceHelper::preload() {
    
    // image
    auto textureCache = Director::getInstance()->getTextureCache();
    
    auto getAnimPNG = [](string anim) -> string {
        return SBStringUtils::replaceAll(anim, ANIM_EXT, ".png");
    };
    
    auto addImageAsync = [textureCache](StringList list) {
      
        for( auto file : list ) {
            textureCache->addImageAsync(file, nullptr);
        }
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
    
    // add character resources
    {
        auto charMgr = CharacterManager::getInstance();
        
        // selected character
        auto chc = charMgr->getSelectedCharacter();
        addImageAsync(chc.idleAnims);
        
        // 각 패키지의 첫번째 캐릭터
        auto packs = charMgr->getPackages();
        
        for( auto pack : packs ) {
            addImageAsync(pack.characters[0].idleAnims);
        }
    }
    
    // sound
    SBAudioEngine::preload(SOUND_BGM_MAIN);
    SBAudioEngine::preload(SOUND_BGM_GAME);
    SBAudioEngine::preload(SOUND_BGM_FEVER);
    SBAudioEngine::preload(SOUND_BGM_CONTINUE);
    SBAudioEngine::preload(SOUND_BGM_RECORD);
    
    SBAudioEngine::preload(SOUND_BUTTON_CLICK);
    SBAudioEngine::preload(SOUND_GAME_OVER);
    SBAudioEngine::preload(SOUND_THUNDER);
    SBAudioEngine::preload(SOUND_SCORE_ROLLING);
    SBAudioEngine::preload(SOUND_LIFE_UP);
    
    SBAudioEngine::preload(SOUND_PUNCH);
    
    for( string file : SOUND_PUNCH_VOICE ) {
        SBAudioEngine::preload(file);
    }
}
