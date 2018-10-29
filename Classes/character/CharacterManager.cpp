//
//  CharacterManager.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 10..
//

#include "CharacterManager.hpp"

#include "json/document-wrapper.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

#include "RSP.h"

USING_NS_CC;
using namespace std;

static const char* SELECTED_CHARACTER                      = "SELECTED_CHARACTER";      // 선택된 캐릭터

static CharacterManager *instance = nullptr;

CharacterManager* CharacterManager::getInstance() {
    
    if( !instance ) {
        instance = new CharacterManager();
    }
    
    return instance;
}

void CharacterManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

CharacterManager::CharacterManager() {
    
}

CharacterManager::~CharacterManager() {
 
    listeners.clear();
    
    for( auto it = packageDbs.begin(); it != packageDbs.end(); ++it ) {
        CC_SAFE_RELEASE_NULL(it->second);
    }
    
    packageDbs.clear();
}

void CharacterManager::init(const string &json) {
    
    CCLOG("CharacterManager::init:\n%s", json.c_str());
    
    // 패키지 json 파싱
    rapidjson::Document doc = SBJSON::parse(json);
    
    // package_order
    auto order = doc["package_order"].GetArray();
    
    for( int i = 0; i < order.Size(); ++i ) {
        packageOrder.push_back(order[i].GetString());
    }
    
    // packages
    auto packList = doc["packages"].GetArray();
    
    for( int i = 0; i < packList.Size(); ++i ) {
        const rapidjson::Value &v = packList[i];
        
        Package pack;
        pack.packId = v["pack_id"].GetString();
        pack.name = v["name"].GetString();
        
        const string DIR = DIR_ADD(DIR_CHARACTER, pack.packId);
        auto charList = v["characters"].GetArray();
        
        for( int j = 0; j < charList.Size(); ++j ) {
            const rapidjson::Value &charValue = charList[j];
            
            Character character;
            character.packId = pack.packId;
            character.charId = charValue["char_id"].GetString();
            character.name = charValue["name"].GetString();
            character.unlockType = (UnlockType)charValue["unlock_type"].GetInt();
            character.unlockAmount = charValue["unlock_amount"].GetInt();
            character.unlockDesc = charValue["unlock_desc"].GetString();
            
            auto getAnims = [=](string key) -> vector<string> {
                
                vector<string> anims;
                int i = 0;
                
                while( true ) {
                    string file = DIR + STR_FORMAT("%s_%s_%02d.png", character.charId.c_str(), key.c_str(), i+1);
                    
                    if( !FileUtils::getInstance()->isFileExist(file) ) {
                        break;
                    }
                    
                    anims.push_back(file);
                    ++i;
                }
                
                return anims;
            };
            
            character.idleAnims = getAnims("idle");
            character.attackAnims = getAnims("attack");
            character.feverGagePos.x = charValue["fever_gage_pos_x"].GetFloat();
            character.feverGagePos.y = charValue["fever_gage_pos_y"].GetFloat();
            
            pack.characters.push_back(character);
            characters[character.charId] = character;
        }
        
        packages.push_back(pack);
        
        CCLOG("%s", pack.toString().c_str());
    }
    
    // DB 초기화
    for( auto pack : packages ) {
        auto packDB = new PackageDB(pack);
        packageDbs[pack.packId] = packDB;
        
        CCLOG("%s", packDB->toString().c_str());
    }
    
    // 기본 캐릭터 설정
    string charId = UserDefault::getInstance()->getStringForKey(SELECTED_CHARACTER, "");
    if( charId == "" ) {
        setSelected(packages[0].characters[0].charId);
    }
}

/**
 * 인덱스에 해당하는 패키지 반환
 */
Package CharacterManager::getPackage(size_t i) {
    
    CCASSERT(i < packages.size(), "CharacterManager::getPackage error: out of range.");
    return packages[i];
}

/**
 * 아이디에 해당하는 패키지 반환
 */
Package CharacterManager::getPackage(const string &packId) {
    
    for( auto pack : packages ) {
        if( pack.packId == packId ) {
            return pack;
        }
    }
    
    return Package();
}

/**
 * 캐릭터 반환
 */
Character CharacterManager::getCharacter(const string &charId) {
    
    if( characters.find(charId) == characters.end() ) {
        CCASSERT(false, "CharacterManager::getCharacter error: invalid character id.");
    }
    
    return characters[charId];
}

/**
 * 선택된 캐릭터(메인 캐릭터) 반환
 */
Character CharacterManager::getSelectedCharacter() {
    
    string charId = UserDefault::getInstance()->getStringForKey(SELECTED_CHARACTER, "");
    
    if( charId == "" ) {
        CCASSERT(false, "CharacterManager::getSelectedCharacter error.");
    }
    
    return getCharacter(charId);
}

/**
 * 패키지 DB 반환
 */
PackageDB* CharacterManager::getPackageDB(const string &packId) {
    
    if( packageDbs.find(packId) == packageDbs.end() ) {
        CCASSERT(false, "CharacterManager::getPackageDB error: invalid package id.");
    }
    
    return packageDbs[packId];
}

/**
 * 캐릭터 선택
 */
void CharacterManager::setSelected(const string &charId) {
    
    CCASSERT(charId != "", "CharacterManager::setSelected error: invalid character id.");
    
    UserDefault::getInstance()->setStringForKey(SELECTED_CHARACTER, charId);
    UserDefault::getInstance()->flush();
}

/**
 * DB 필드 값 업데이트
 */
void CharacterManager::submit(PackageDB::Field field, int i, PackageDB::OnCharacterUnlocked onCharacterUnlocked) {
    
    for( auto it = packageDbs.begin(); it != packageDbs.end(); ++it ) {
        it->second->submit(field, i, onCharacterUnlocked);
    }
}

///**
// * 패키지 잠금 해제
// */
//void CharacterManager::unlockPackage(const string &packId) {
//
//    auto pack = getPackage(packId);
//
//    for( auto character : pack.characters ) {
//        unlockCharacter(character.charId);
//    }
//}
//
///**
// * 캐릭터 잠금 해제
// */
//void CharacterManager::unlockCharacter(const string &charId) {
//
//    if( getCharacter(charId).charId == "" ) {
//        CCASSERT(false, "CharacterManager::unlockCharacter error: invalid character id.");
//        return;
//    }
//
//    // json 오브젝트 생성
//    rapidjson::Document doc;
//    doc.SetArray();
//
//    rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
//
//    // 이전 데이터 추가
//    string data = UserDefault::getInstance()->getStringForKey(UNLOCK_CHARACTERS, "");
//
//    if( data != "" ) {
//        auto charList = SBJSON::parse(data).GetArray();
//
//        for( int i = 0; i < charList.Size(); ++i ) {
//            doc.PushBack(SBJSON::value(charList[i].GetString(), allocator), allocator);
//        }
//    }
//
//    // 새로운 캐릭터 추가
//    bool isContains = false;
//
//    for( int i = 0; i < doc.Size(); ++i ) {
//        string unlockCharId = doc[i].GetString();
//
//        if( unlockCharId == charId ) {
//            isContains = true;
//            break;
//        }
//    }
//
//    if( !isContains ) {
//        doc.PushBack(SBJSON::value(charId, allocator), allocator);
//    }
//
//    // 잠금 해제된 캐릭터 리스트 저장
//    rapidjson::StringBuffer strbuf;
//    strbuf.Clear();
//
//    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
//    doc.Accept(writer);
//
//    string newData = strbuf.GetString();
//
//    UserDefault::getInstance()->setStringForKey(UNLOCK_CHARACTERS, newData);
//    UserDefault::getInstance()->flush();
//
//    CCLOG("unlockCharacter: %s newData: %s", charId.c_str(), newData.c_str());
//}

/**
 * 패키지 잠금 해제
 */
void CharacterManager::onPackageUnlocked(const vector<string> &packages) {
    
    for( auto listener : listeners ) {
        if( listener->onPackageUnlocked ) {
            listener->onPackageUnlocked(packages);
        }
    }
}

/**
 * 캐릭터 잠금 해제
 */
void CharacterManager::onCharacterUnlocked(const vector<string> &characters) {
    
    for( auto listener : listeners ) {
        if( listener->onCharacterUnlocked ) {
            listener->onCharacterUnlocked(characters);
        }
    }
}

/**
 * 리스너 등록
 */
void CharacterManager::addListener(CharacterListener *listener) {
    
    CCASSERT(listener != nullptr, "CharacterManager::addListener error: listener must be not null");
    
    if( !listeners.contains(listener) ) {
        listeners.pushBack(listener);
    }
}

/**
 * 리스너 제거
 */
void CharacterManager::removeListener(CharacterListener *listener) {
    
    CCASSERT(listener != nullptr, "CharacterManager::addListener error: listener must be not null");
    
    if( listeners.contains(listener) ) {
        listeners.eraseObject(listener);
    }
}

void CharacterManager::removeListener(Ref *target) {
    
    CCASSERT(target != nullptr, "CharacterManager::removeListener error: target must be not null");
    
    auto removeListeners = SBCollection::find(listeners, [=](CharacterListener *listener) -> bool {
        return listener->getTarget() == target;
    });
    
    for( auto listener : removeListeners ) {
        listeners.eraseObject(listener);
    }
}


