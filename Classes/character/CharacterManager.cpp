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

#include "Define.h"
#include "CharacterResourceHelper.hpp"

USING_NS_CC;
USING_NS_SB;
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
    
    initIAPListener();
}

CharacterManager::~CharacterManager() {
 
    iap::IAPHelper::getInstance()->removeListener(this);
    
    listeners.clear();
    
    for( auto it = packageDbs.begin(); it != packageDbs.end(); ++it ) {
        CC_SAFE_RELEASE_NULL(it->second);
    }
    
    packageDbs.clear();
}

/**
 * 인앱 결제 리스너 초기화
 */
void CharacterManager::initIAPListener() {
    
    // purchase listener
//    auto purchaseListener = iap::PurchaseListener::create();
//    purchaseListener->setForever(true);
//    purchaseListener->onPurchased
//    purchaseListener->onRemoveAds = onRemoveAds;
//
//    iap::IAPHelper::getInstance()->addListener(this, purchaseListener);
    
    // restore listener
    auto restoreListener = iap::RestoreListener::create();
    restoreListener->setTarget(this);
    restoreListener->setForever(true);
    restoreListener->onPurchased = [=](const iap::Item &item) {
        
        CCLOG("CharacterManager RestoreListener::onPurchased: %s", item.name.c_str());
        
        bool found = false;
        
        // 패키지 복원 체크
        for( auto pack : packages ) {
            if( pack.packId == item.name ) {
                this->unlockPackage(pack.packId, true);
                found = true;
                break;
            }
        }
        
        // 캐릭터 복원 체크
        if( !found ) {
            for( auto it : characters ) {
                auto charId = it.first;
                
                if( charId == item.name ) {
                    this->unlockCharacter(charId, true);
                    break;
                }
            }
        }
    };
    
    iap::IAPHelper::getInstance()->addListener(restoreListener);
}

void CharacterManager::init(const string &json) {
    
    CCLOG("CharacterManager::init:\n%s", json.c_str());
    CCLOG("========== PARSE START (character_package.json)  ==========");
    
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
            
            // 테스트를 위한 amount 조절
            /*
            if( character.unlockType == UnlockType::VIEW_ADS ) {
                character.unlockAmount = 1;
            } else {
                character.unlockAmount /= 3;
            }

            character.unlockAmount = MAX(1, character.unlockAmount);
             */
            /////////////////////////////
            
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
            
            character.idleAnims = CHC_IDLE_ANIMATION(character);
            character.attackAnims = CHC_ATTACK_ANIMATION(character);
            character.punchSound = CHC_PUNCH_SOUND(character);
            
            character.feverGagePos.x = charValue["fever_gage_pos_x"].GetFloat();
            character.feverGagePos.y = charValue["fever_gage_pos_y"].GetFloat();
            
            pack.characters.push_back(character);
            characters[character.charId] = character;
        }
        
        packages.push_back(pack);
        
        CCLOG("%s", pack.toString().c_str());
    }
    
    CCLOG("========== PARSE END (character_package.json)  ==========");
    
    // DB 초기화
    CCLOG("================ CHARACTER DB INIT  ================");
    for( auto pack : packages ) {
        auto packDB = new PackageDB(pack);
        packageDbs[pack.packId] = packDB;
        
        CCLOG("%s", packDB->toString().c_str());
    }
    
    CCLOG("============== CHARACTER DB INIT END  ==============");
    
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
 * 패키지 잠금 해제 여부를 반환 합니다
 */
bool CharacterManager::isPackageUnlocked(const string &packId) {
    
    return getPackageDB(packId)->isPackageUnlocked();
}

/**
 * 캐릭터 잠금 해제 여부를 반환 합니다
 */
bool CharacterManager::isCharacterUnlocked(const string &charId) {
    
    for( auto it = packageDbs.begin(); it != packageDbs.end(); ++it ) {
        auto db = it->second;
        if( db->isCharacterUnlocked(charId) ) {
            return true;
        }
    }
    
    return false;
}

/**
 * 캐릭터 선택
 */
void CharacterManager::setSelected(const string &charId) {
    
    CCASSERT(charId != "", "CharacterManager::setSelected error: invalid character id.");
    
    UserDefault::getInstance()->setStringForKey(SELECTED_CHARACTER, charId);
    UserDefault::getInstance()->flush();
    
    onCharacterSelected(getCharacter(charId));
}

/**
 * 패키지 잠금 해제
 */
void CharacterManager::unlockPackage(const string &packId, bool isRestored) {
    
    auto db = getPackageDB(packId);
    
    if( db->isPackageUnlocked() ) {
        return;
    }
    
    auto newUnlockCharacters = db->getLockedCharacters();
    
    db->unlockPackage();
    db->commit();
    
    if( !isRestored ) {
        onCharacterUnlocked(newUnlockCharacters);
        onPackageUnlocked(Packages({db->getPackage()}));
    } else {
        onCharacterRestored(newUnlockCharacters);
        onPackageRestored(Packages({db->getPackage()}));
    }
}

/**
 * 캐릭터 잠금 해제
 */
void CharacterManager::unlockCharacter(const string &charId, bool isRestored) {
    
    auto character = getCharacter(charId);
    auto db = getPackageDB(character.packId);
    
    if( db->isCharacterUnlocked(charId) ) {
        return;
    }
    
    db->unlockCharacter(charId);
    db->commit();
    
    if( !isRestored ) {
        onCharacterUnlocked(Characters({character}));
    } else {
        onCharacterRestored(Characters({character}));
    }
}

/**
 * DB 커밋
 */
void CharacterManager::commit(const string &packId) {
 
    getPackageDB(packId)->commit();
}

void CharacterManager::commitAll() {
    
    for( auto it = packageDbs.begin(); it != packageDbs.end(); ++it ) {
        auto db = it->second;
        db->commit();
    }
}

/**
 * DB 필드 값 업데이트
 * 새로 해제된 패키지, 캐릭터를 리스너로 전달합니다
 */
void CharacterManager::submit(CharacterListener listener, PackageDB::Field field, int i, const string &charId) {
    
    Characters *unlockCharacters = new Characters();
    Packages *unlockPackages = new Packages();
    
    auto submit = [=](PackageDB *db) {
        
        db->submit([=](Characters characters) {
            
            for( auto character : characters ) {
                unlockCharacters->push_back(character);
            }
            
            if( db->isPackageUnlocked() ) {
                unlockPackages->push_back(db->getPackage());
            }
            
        }, field, i, charId);
    };
    
    // 기본 필드
    if( field != PackageDB::Field::VIEW_ADS ) {
        for( auto it = packageDbs.begin(); it != packageDbs.end(); ++it ) {
            auto db = it->second;
            submit(db);
        }
    }
    // VIEW_ADS 필드
    else {
        if( charId == "" ) {
            CCASSERT(false, "CharacterManager::submit error: invalid character id.");
            return;
        }
        
        auto db = getPackageDB(getCharacter(charId).packId);
        submit(db);
    }
    
    if( unlockCharacters->size() > 0 ) {
        if( listener.onCharacterUnlocked ) {
            listener.onCharacterUnlocked(*unlockCharacters);
        }
        
        onCharacterUnlocked(*unlockCharacters);
    }
    
    if( unlockPackages->size() > 0 ) {
        if( listener.onPackageUnlocked ) {
            listener.onPackageUnlocked(*unlockPackages);
        }
        
        onPackageUnlocked(*unlockPackages);
    }
    
    delete unlockCharacters;
    delete unlockPackages;
}

void CharacterManager::submit(PackageDB::Field field, int i, const string &charId) {
 
    submit(CharacterListener(), field, i, charId);
}

/**
 * 캐릭터 선택됨
 */
void CharacterManager::onCharacterSelected(const Character &character) {
    
    CCLOG("CharacterManager::onCharacterSelected: %s", character.charId.c_str());
    
    for( auto listener : listeners ) {
        if( listener->onCharacterSelected ) {
            listener->onCharacterSelected(character);
        }
    }
}

/**
 * 패키지 잠금 해제
 */
void CharacterManager::onPackageUnlocked(const Packages &packages) {
    
    CCLOG("CharacterManager::onPackageUnlocked: %s", packIdToString(getPackageIds(packages)).c_str());
    
    for( auto listener : listeners ) {
        if( listener->onPackageUnlocked ) {
            listener->onPackageUnlocked(packages);
        }
    }
}

/**
 * 캐릭터 잠금 해제
 */
void CharacterManager::onCharacterUnlocked(const Characters &characters) {
    
    CCLOG("CharacterManager::onCharacterUnlocked: %s", charIdToString(getCharacterIds(characters)).c_str());
    
    for( auto listener : listeners ) {
        if( listener->onCharacterUnlocked ) {
            listener->onCharacterUnlocked(characters);
        }
    }
}

/**
 * 패키지 IAP 복원
 */
void CharacterManager::onPackageRestored(const Packages &packages) {
    
    CCLOG("CharacterManager::onPackageRestored: %s", packIdToString(getPackageIds(packages)).c_str());
    
    for( auto listener : listeners ) {
        if( listener->onPackageRestored ) {
            listener->onPackageRestored(packages);
        }
    }
}

/**
 * 캐릭터 IAP 복원
 */
void CharacterManager::onCharacterRestored(const Characters &characters) {
    
    CCLOG("CharacterManager::onCharacterRestored: %s", charIdToString(getCharacterIds(characters)).c_str());
    
    for( auto listener : listeners ) {
        if( listener->onCharacterRestored ) {
            listener->onCharacterRestored(characters);
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


