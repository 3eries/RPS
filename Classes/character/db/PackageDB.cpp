//
//  PackageDB.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 16..
//

#include "PackageDB.hpp"

#include "json/document-wrapper.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

#include "RSP.h"

USING_NS_CC;
using namespace std;

#define KEY_PACK_ID                     "pack_id"
#define KEY_UNLOCK                      "unlock"
#define KEY_BEST_SCORE                  "best_score"
#define KEY_TOTAL_SCORE                 "total_score"
#define KEY_GAME_PLAY                   "game_play"
#define KEY_GAME_OVER                   "game_over"
#define KEY_DAILY_LOGIN                 "daily_login"
#define KEY_FEVER                       "fever"
#define KEY_VIEW_ADS                    "view_ads"
#define KEY_UNLOCK_CHARACTERS           "unlock_characters"

static const char* getDBKey(Package pack) {
    return STR_FORMAT("character_%s", pack.packId.c_str()).c_str();
}

static const char* getFieldKey(PackageDB::Field field) {
    
    switch( field ) {
        case PackageDB::Field::BEST_SCORE:      return KEY_BEST_SCORE;
        case PackageDB::Field::TOTAL_SCORE:     return KEY_TOTAL_SCORE;
        case PackageDB::Field::GAME_PLAY:       return KEY_GAME_PLAY;
        case PackageDB::Field::GAME_OVER:       return KEY_GAME_OVER;
        case PackageDB::Field::DAILY_LOGIN:     return KEY_DAILY_LOGIN;
        case PackageDB::Field::FEVER:           return KEY_FEVER;
        case PackageDB::Field::VIEW_ADS:        return KEY_VIEW_ADS;
        default:
            CCASSERT(false, "getFieldKey error.");
            break;
    }
    
    return "";
}

static UnlockType getCharacterUnlockType(PackageDB::Field field) {
    
    switch( field ) {
        case PackageDB::Field::BEST_SCORE:      return UnlockType::BEST_SCORE;
        case PackageDB::Field::TOTAL_SCORE:     return UnlockType::TOTAL_SCORE;
        case PackageDB::Field::GAME_PLAY:       return UnlockType::GAME_PLAY;
        case PackageDB::Field::GAME_OVER:       return UnlockType::GAME_OVER;
        case PackageDB::Field::DAILY_LOGIN:     return UnlockType::DAILY_LOGIN;
        case PackageDB::Field::FEVER:           return UnlockType::FEVER;
        case PackageDB::Field::VIEW_ADS:        return UnlockType::VIEW_ADS;
        default:
            CCASSERT(false, "getCharacterUnlockType error.");
            break;
    }
    
    return UnlockType::NONE;
}

PackageDB::PackageDB(Package pack) :
pack(pack),
isUnlocked(false) {
    
    Field fields[] = {
        Field::BEST_SCORE,
        Field::TOTAL_SCORE,
        Field::GAME_PLAY,
        Field::GAME_OVER,
        Field::DAILY_LOGIN,
        Field::FEVER,
        Field::VIEW_ADS,
    };
    
    for( auto field : fields ) {
        fieldValues[field] = 0;
    }
    
    string data = UserDefault::getInstance()->getStringForKey(getDBKey(pack), "");
    CCLOG("PackageDB packId: %s, data:\n%s", pack.packId.c_str(), data.c_str());
    
    // 기존 데이터 있음
    if( data != "" ) {
        rapidjson::Document doc = SBJSON::parse(data);
        
        isUnlocked = doc[KEY_UNLOCK].GetBool();
        
        for( auto it = fieldValues.begin(); it != fieldValues.end(); ++it ) {
            auto field = it->first;
            fieldValues[field] = doc[getFieldKey(field)].GetInt();
        }
        
        auto unlockCharList = doc[KEY_UNLOCK_CHARACTERS].GetArray();
        
        for( int i = 0; i < unlockCharList.Size(); ++i ) {
            unlockCharacters.push_back(unlockCharList[i].GetString());
        }
    }
}

PackageDB::~PackageDB() {
    
}

string PackageDB::toString() {
    
    std::string str = "PackageDB {\n";
    str += "\t" + STR_FORMAT("packId: %s", pack.packId.c_str()) + "\n";
    str += "\t" + STR_FORMAT("isUnlocked: %d", isUnlocked) + "\n";
    
    for( auto it = fieldValues.begin(); it != fieldValues.end(); ++it ) {
        str += "\t" + STR_FORMAT("%s: %d", getFieldKey(it->first), it->second) + "\n";
    }

    str += "\tunlockCharacters: ";
    
    for( string charId : unlockCharacters ) {
        str += charId + ", ";
    }

    str += "\n}";

    return str;
}

/**
 * 데이터 커밋, UserDefault에 현재 값을 저장
 */
void PackageDB::commit() {
    
    // data to json
    rapidjson::Document doc;
    doc.SetObject();
    
    rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
    
    // package unlock
    doc.AddMember(KEY_UNLOCK, isUnlocked, allocator);
    
    // field
    for( auto it = fieldValues.begin(); it != fieldValues.end(); ++it ) {
        string key = getFieldKey(it->first);
        doc.AddMember(SBJSON::value(key, allocator), rapidjson::Value(it->second), allocator);
    }
    
    // unlock characters
    rapidjson::Value unlockCharValue(rapidjson::kArrayType);
    
    for( auto charId : unlockCharacters ) {
        unlockCharValue.PushBack(SBJSON::value(charId, allocator), allocator);
    }
    
    doc.AddMember(KEY_UNLOCK_CHARACTERS, unlockCharValue, allocator);
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    // write
    string json = strbuf.GetString();
    CCLOG("PackageDB::commit:\n%s", json.c_str());
    
//    UserDefault::getInstance()->setStringForKey(getDBKey(pack), json);
//    UserDefault::getInstance()->flush();
}

/**
 * 패키지 잠금해제 여부 반환
 */
bool PackageDB::isPackageUnlocked() {
    
    return isUnlocked;
}

/**
 * 캐릭터 잠금해제 여부 반환
 */
bool PackageDB::isCharacterUnlocked(const string &charId) {
    
    for( auto c : unlockCharacters ) {
        if( c == charId ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 패키지 잠금 해제
 */
void PackageDB::unlockPackage() {
    
    isUnlocked = true;
    
    for( auto c : pack.characters ) {
        unlockCharacter(c.charId);
    }
}

/**
 * 캐릭터 잠금 해제
 */
void PackageDB::unlockCharacter(const string &charId) {
    
    if( charId == "" ) {
        CCASSERT(false, "PackageDB::unlockCharacter error: invalid character id.");
        return;
    }
    
    bool found = false;
    
    for( auto c : unlockCharacters ) {
        if( c == charId ) {
            found = true;
            break;
        }
    }
    
    if( !found ) {
        unlockCharacters.push_back(charId);
    }
    
    // 모든 캐릭터 해제됐는지 체크
    if( pack.characters.size() == unlockCharacters.size() ) {
        isUnlocked = true;
    }
}


/**
 * 필드 값 업데이트
 */
void PackageDB::submit(Field field, int i, OnCharacterUnlocked onCharacterUnlocked) {
    
    auto it = fieldValues.find(field);
    if( it == fieldValues.end() ) {
        CCASSERT(false, "PackageDB::submit error: invalid field.");
        return;
    }
    
    int value = (it->second + i);
    fieldValues[field] = value;
    
    // 잠금 해제된 캐릭터 체크
    const auto unlockType = getCharacterUnlockType(field);
    vector<string> newUnlockCharacters;
    
    auto isUnlocked = [=](Character c) -> bool {
        return c.unlockType == unlockType && value >= c.unlockAmount;
    };
    
    for( auto c : pack.characters ) {
        if( isCharacterUnlocked(c.charId) ) {
            continue;
        }
        
        if( isUnlocked(c) ) {
            unlockCharacter(c.charId);
            newUnlockCharacters.push_back(c.charId);
        }
    }
    
    if( onCharacterUnlocked && newUnlockCharacters.size() > 0 ) {
        onCharacterUnlocked(newUnlockCharacters);
    }
}

