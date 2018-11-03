//
//  CharacterListener.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 26..
//

#include "CharacterListener.hpp"

CharacterListener::CharacterListener() :
target(nullptr),
onCharacterSelected(nullptr),
onPackageUnlocked(nullptr),
onCharacterUnlocked(nullptr),
onPackageRestored(nullptr),
onCharacterRestored(nullptr) {
}

CharacterListener::~CharacterListener() {
}
