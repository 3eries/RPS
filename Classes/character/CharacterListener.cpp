//
//  CharacterListener.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 26..
//

#include "CharacterListener.hpp"

CharacterListener::CharacterListener() :
target(nullptr),
onPackageUnlocked(nullptr),
onCharacterUnlocked(nullptr) {
}

CharacterListener::~CharacterListener() {
}
