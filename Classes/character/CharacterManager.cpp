//
//  CharacterManager.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 10..
//

#include "CharacterManager.hpp"

USING_NS_CC;
using namespace std;

static CharacterManager *instance = nullptr;

CharacterManager* CharacterManager::getInstance() {
    
    if( !instance ) {
        instance = new CharacterManager();
    }
    
    return instance;
}

CharacterManager::CharacterManager() {
    
}

CharacterManager::~CharacterManager() {
    
}

void CharacterManager::init(const string &json) {
    
    CCLOG("CharacterManager::init:\n%s", json.c_str());
    
    rapidjson::Document doc = SBJSON::parse(json);
    auto packList = doc.GetArray();
    
    for( int i = 0; i < packList.Size(); ++i ) {
        auto it = packList[i].MemberBegin();
        auto charList = it->value.GetArray();
        
        Package pack;
        pack.packId = it->name.GetString();
        
        for( int j = 0; j < charList.Size(); ++j ) {
            const rapidjson::Value &charValue = charList[j];
            
            Character character;
            character.charId = charValue["char_id"].GetString();
            character.name = charValue["name"].GetString();
            character.unlockType = (UnlockType)charValue["unlock_type"].GetInt();
            character.unlockAmount = charValue["unlock_amount"].GetInt();
            character.unlockDesc = charValue["unlock_desc"].GetString();
            
            pack.characters.push_back(character);
        }
        
        packages.push_back(pack);
        
        CCLOG("%s", pack.toString().c_str());
    }
}

Package CharacterManager::getPackage(size_t i) {
    
    CCASSERT(i < packages.size(), "CharacterManager::getPackage error: out of range.");
    return packages[i];
}

