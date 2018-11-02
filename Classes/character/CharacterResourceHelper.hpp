//
//  CharacterResourceHelper.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 11. 2..
//

#ifndef CharacterResourceHelper_hpp
#define CharacterResourceHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "model/Package.h"

#define CHC_IDLE_ANIMATION(__chc__)      CharacterResourceHelper::getIdleAnimationFiles(__chc__)
#define CHC_ATTACK_ANIMATION(__chc__)    CharacterResourceHelper::getAttackAnimationFiles(__chc__)
#define CHC_PUNCH_SOUND(__chc__)         CharacterResourceHelper::getPunchSoundFile(__chc__)

class CharacterResourceHelper {
public:
    static std::string getRootDir(const Character &character);
    static StringList  getAnimationFiles(const Character &character,
                                         const std::string &key);
    static StringList  getIdleAnimationFiles(const Character &character);
    static StringList  getAttackAnimationFiles(const Character &character);
    static std::string getPunchSoundFile(const Character &character);
};

#endif /* CharacterResourceHelper_hpp */
