//
//  CharacterResourceHelper.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 11. 2..
//

#include "CharacterResourceHelper.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

string CharacterResourceHelper::getRootDir(const Character &character) {
    
    return DIR_ADD(DIR_CHARACTER, character.packId);
}

StringList CharacterResourceHelper::getAnimationFiles(const Character &character, const string &key) {
    
    StringList anims;
    int i = 0;

    const string DIR = getRootDir(character);
    
    while( true ) {
        string file = DIR + STR_FORMAT("%s_%s_%02d.png", character.charId.c_str(), key.c_str(), i+1);

        if( !FileUtils::getInstance()->isFileExist(file) ) {
            break;
        }

        anims.push_back(file);
        ++i;
    }

    return anims;
}

StringList CharacterResourceHelper::getIdleAnimationFiles(const Character &character) {
    
    return getAnimationFiles(character, "idle");
}

StringList CharacterResourceHelper::getAttackAnimationFiles(const Character &character) {
    
    return getAnimationFiles(character, "attack");
}

string CharacterResourceHelper::getPunchSoundFile(const Character &character) {
    
    string file = getRootDir(character) + STR_FORMAT("%s_snd_punch.mp3", character.charId.c_str());
    
    if( FileUtils::getInstance()->isFileExist(file) ) {
        return file;
    }
    
    return "";
}


