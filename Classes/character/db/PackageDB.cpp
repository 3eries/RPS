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

#include "Define.h"

USING_NS_CC;
using namespace std;

#define KEY_PACK_ID                     "pack_id"
#define KEY_BEST_SCORE                  "best_score"
#define KEY_TOTAL_SCORE                 "total_score"
#define KEY_GAME_PLAY                   "game_play"
#define KEY_GAME_OVER                   "game_over"
#define KEY_DAILY_LOGIN                 "daily_login"
#define KEY_FEVER                       "fever"
#define KEY_VIEW_ADS                    "view_ads"
#define KEY_UNLOCK_CHARACTERS           "unlock_characters"

static string getDBKey(Package pack) {
    return STR_FORMAT("character_%s", pack.packId.c_str());
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
pack(pack) {
    
    Field fields[] = {
        Field::BEST_SCORE,
        Field::TOTAL_SCORE,
        Field::GAME_PLAY,
        Field::GAME_OVER,
        Field::DAILY_LOGIN,
        Field::FEVER,
    };
    
    for( auto field : fields ) {
        fieldValues[field] = 0;
    }
    
    for( auto c : pack.characters ) {
        if( c.unlockType == UnlockType::VIEW_ADS ) {
            viewAdsValues[c.charId] = 0;
        }
    }
    
    string data = UserDefault::getInstance()->getStringForKey(getDBKey(pack).c_str(), "");
    CCLOG("PackageDB packId: %s, data:\n%s", pack.packId.c_str(), data.c_str());
    
    // 기존 데이터 있음
    if( data != "" ) {
        rapidjson::Document doc = SBJSON::parse(data);
        
        // field
        for( auto it = fieldValues.begin(); it != fieldValues.end(); ++it ) {
            auto field = it->first;
            fieldValues[field] = doc[getFieldKey(field)].GetInt();
        }
        
        // view ads field
        auto viewAdsList = doc[KEY_VIEW_ADS].GetArray();
        
        for( int i = 0; i < viewAdsList.Size(); ++i ) {
            auto it = viewAdsList[i].MemberBegin();
            viewAdsValues[it->name.GetString()] = it->value.GetInt();
        }
        
        // unlock characters
        auto unlockCharList = doc[KEY_UNLOCK_CHARACTERS].GetArray();
        
        for( int i = 0; i < unlockCharList.Size(); ++i ) {
            unlockCharacters.push_back(unlockCharList[i].GetString());
        }
        
        // 잠금 해제된 캐릭터 체크
        // 조건은 충족했지만 예외가 발생해, 해제되지 않은 캐릭터가 있을까 하여 추가 체크
        Characters newUnlockCharacters;
        
        for( auto c : pack.characters ) {
            // 이미 해제된 캐릭터 PASS
            if( isCharacterUnlocked(c.charId) ) {
                continue;
            }
            
            // 잠금 해제 체크
            if( getUnlockFieldValue(c) >= c.unlockAmount ) {
                unlockCharacter(c.charId);
                newUnlockCharacters.push_back(c);
            }
        }
        
        if( newUnlockCharacters.size() > 0 ) {
            commit();
        }
    }
    // 기존 데이터 없음
    else {
        // 잠금 해제 타입이 없는 캐릭터 해제
        for( auto c : pack.characters ) {
            if( c.unlockType == UnlockType::NONE ) {
                unlockCharacter(c.charId);
            }
        }
        
        if( unlockCharacters.size() > 0 ) {
            commit();
        }
    }
}

PackageDB::~PackageDB() {
    
}

string PackageDB::toString() {
    
    std::string str = "PackageDB {\n";
    str += "\t" + STR_FORMAT("packId: %s", pack.packId.c_str()) + "\n";
    str += "\t" + STR_FORMAT("isUnlocked: %d", isPackageUnlocked()) + "\n";
    
    for( auto it = fieldValues.begin(); it != fieldValues.end(); ++it ) {
        str += "\t" + STR_FORMAT("%s: %d", getFieldKey(it->first), it->second) + "\n";
    }
    
    for( auto it = viewAdsValues.begin(); it != viewAdsValues.end(); ++it ) {
        str += "\t" + STR_FORMAT("viewAds %s: %d", it->first.c_str(), it->second) + "\n";
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
    
    CCLOG("========== PackageDB COMMIT  ==========");
    CCLOG("packId: %s", pack.packId.c_str());
    
    double nowTime = SBSystemUtils::getCurrentTimeMillis();
    
    // data to json
    rapidjson::Document doc;
    doc.SetObject();
    
    rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
    
    // field
    for( auto it = fieldValues.begin(); it != fieldValues.end(); ++it ) {
        string key = getFieldKey(it->first);
        doc.AddMember(SBJSON::value(key, allocator), rapidjson::Value(it->second), allocator);
    }
    
    // view ads field
    rapidjson::Value viewAdsList(rapidjson::kArrayType);
    
    for( auto it = viewAdsValues.begin(); it != viewAdsValues.end(); ++it ) {
        rapidjson::Value v(rapidjson::kObjectType);
        v.AddMember(SBJSON::value(it->first, allocator), rapidjson::Value(it->second), allocator);
        
        viewAdsList.PushBack(v, allocator);
    }
    
    doc.AddMember(KEY_VIEW_ADS, viewAdsList, allocator);
    
    // unlock characters
    rapidjson::Value unlockCharValue(rapidjson::kArrayType);
    
    for( auto charId : unlockCharacters ) {
        unlockCharValue.PushBack(SBJSON::value(charId, allocator), allocator);
    }
    
    doc.AddMember(KEY_UNLOCK_CHARACTERS, unlockCharValue, allocator);
    
    // to json string
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);
    
    // write
    string json = strbuf.GetString();
    CCLOG("PackageDB::commit:\n%s", json.c_str());
    
    UserDefault::getInstance()->setStringForKey(getDBKey(pack).c_str(), json);
    UserDefault::getInstance()->flush();
    
    CCLOG("dt: %f", (SBSystemUtils::getCurrentTimeMillis() - nowTime) / 1000);
    CCLOG("========= PackageDB COMMIT END =========");
}

/**
 * 잠겨 있는 캐릭터 반환
 */
Characters PackageDB::getLockedCharacters() {
    
    Characters lockedCharacters;
    
    for( auto character : pack.characters ) {
        if( !isCharacterUnlocked(character.charId) ) {
            lockedCharacters.push_back(character);
        }
    }
    
    return lockedCharacters;
}

/**
 * 패키지 잠금해제 여부 반환
 */
bool PackageDB::isPackageUnlocked() {
    
    return (unlockCharacters.size() == pack.characters.size());
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
 * 필드 값 반환
 */
int PackageDB::getFieldValue(Field f) {
    
    auto it = fieldValues.find(f);
    if( it == fieldValues.end() ) {
        CCASSERT(false, "PackageDB::getFieldValue error");
        return 0;
    }
    
    return it->second;
}

/**
 * 광고 시청 값 반환
 */
int PackageDB::getViewAdsValue(const string &charId) {
    
    auto it = viewAdsValues.find(charId);
    if( it == viewAdsValues.end() ) {
        CCASSERT(false, "PackageDB::getViewAdsValue error");
        return 0;
    }
    
    return it->second;
}

/**
 * 캐릭터 잠금 해제 조건에 해당하는 필드 값 반환
 */
int PackageDB::getUnlockFieldValue(Character c) {
    
    // 잠금 해제된 경우, 조건 값을 반환
    if( isCharacterUnlocked(c.charId) ) {
        return c.unlockAmount;
    }
    
    switch( c.unlockType ) {
        case UnlockType::BEST_SCORE:      return getFieldValue(Field::BEST_SCORE);
        case UnlockType::TOTAL_SCORE:     return getFieldValue(Field::TOTAL_SCORE);
        case UnlockType::GAME_PLAY:       return getFieldValue(Field::GAME_PLAY);
        case UnlockType::GAME_OVER:       return getFieldValue(Field::GAME_OVER);
        case UnlockType::DAILY_LOGIN:     return getFieldValue(Field::DAILY_LOGIN);
        case UnlockType::FEVER:           return getFieldValue(Field::FEVER);
        case UnlockType::VIEW_ADS:        return getViewAdsValue(c.charId);
        default:
            CCASSERT(false, "PackageDB::getUnlockFieldValue error.");
            break;
    }
    
    return 0;
}

/**
 * 패키지 잠금 해제
 */
void PackageDB::unlockPackage() {
    
    if( !isPackageUnlocked() ) {
        for( auto c : pack.characters ) {
            unlockCharacter(c.charId);
        }
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
}

/**
 * 필드 값 업데이트
 */
void PackageDB::submit(OnCharacterListListener listener, Field field, int i, const string &charId) {
    
    // 잠금 해제된 패키지는 필드 값을 업데이트 하지 않는다
    if( isPackageUnlocked() ) {
        return;
    }
    
    // 필드에 해당하는 캐릭터가 모두 잠금 해제된 경우 값을 업데이트 하지 않는다
    const auto unlockType = getCharacterUnlockType(field);
    auto lockedCharacters = SBCollection::find(getLockedCharacters(), [=](Character chc) -> bool {
        return chc.unlockType == unlockType;
    });
    
    if( lockedCharacters.size() == 0 ) {
        return;
    }
    
    const bool isViewAdsField = (field == Field::VIEW_ADS);
    int fieldValue = 0;
    
    // 기본 필드
    if( !isViewAdsField ) {
        auto it = fieldValues.find(field);
        if( it == fieldValues.end() ) {
            CCASSERT(false, "PackageDB::submit error: invalid field.");
            return;
        }
        
        fieldValue = it->second;
        
        switch( field ) {
            case Field::TOTAL_SCORE:
            case Field::GAME_PLAY:
            case Field::GAME_OVER:
            case Field::DAILY_LOGIN:
            case Field::FEVER: {
                fieldValue += i;
            } break;
                
            case Field::BEST_SCORE: {
                fieldValue = MAX(i, fieldValue);
            } break;
                
            default: break;
        }
        
        fieldValues[field] = fieldValue;
    }
    // VIEW_ADS 필드
    else {
        if( charId == "" ) {
            CCASSERT(false, "PackageDB::submit error: invalid character id.");
            return;
        }
        
        fieldValue = getViewAdsValue(charId) + i;
        viewAdsValues[charId] = fieldValue;
    }
    
    // 잠금 해제된 캐릭터 체크
    Characters newUnlockCharacters;
    
    auto isUnlocked = [=](Character c) -> bool {
        return c.unlockType == unlockType && fieldValue >= c.unlockAmount;
    };
    
    for( auto c : pack.characters ) {
        // 이미 해제된 캐릭터 PASS
        if( isCharacterUnlocked(c.charId) ) {
            continue;
        }
        
        // VIEW_ADS 필드 & 다른 캐릭터 PASS
        if( isViewAdsField && charId != c.charId ) {
            continue;
        }
        
        // 잠금 해제 체크
        if( isUnlocked(c) ) {
            unlockCharacter(c.charId);
            newUnlockCharacters.push_back(c);
        }
    }
    
    // 리스너 실행
    if( newUnlockCharacters.size() > 0 ) {
        if( listener ) {
            listener(newUnlockCharacters);
        }
    }
}

