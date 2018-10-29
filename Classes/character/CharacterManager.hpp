//
//  CharacterManager.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 10..
//

#ifndef CharacterManager_hpp
#define CharacterManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "model/Package.h"
#include "db/PackageDB.hpp"

#include "CharacterListener.hpp"

class CharacterManager {    
public:
    static CharacterManager* getInstance();
    static void destroyInstance();
    ~CharacterManager();
    
private:
    CharacterManager();
    
public:
    void init(const std::string &json);
    
    Package                    getPackage(size_t i);
    Package                    getPackage(const std::string &packId);
    Character                  getCharacter(const std::string &charId);
    Character                  getSelectedCharacter();
    PackageDB*                 getPackageDB(const std::string &packId);
    
    void setSelected(const std::string &charId);
    
//    void unlockPackage(const std::string &packId);
//    void unlockCharacter(const std::string &charId);
    
    void submit(PackageDB::Field field, int i = 1,
                PackageDB::OnCharacterUnlocked onCharacterUnlocked = nullptr);
    
private:
    // 패키지 노출 순서
    CC_SYNTHESIZE_READONLY(std::vector<std::string>, packageOrder, PackageOrder);
    
    // 패키지 리스트
    CC_SYNTHESIZE_READONLY(std::vector<Package>, packages, Packages);
    
    // 모든 캐릭터 리스트
    std::map<std::string, Character> characters;

    // 패키지 별 DB
    std::map<std::string, PackageDB*> packageDbs;
    
public:
    // 패키지 잠금 해제
    void onPackageUnlocked(const std::vector<std::string> &packages);
    // 캐릭터 잠금 해제
    void onCharacterUnlocked(const std::vector<std::string> &characters);
    
public:
    void addListener(CharacterListener *listener);
    void removeListener(CharacterListener *listener);
    void removeListener(cocos2d::Ref *target);
    
private:
    cocos2d::Vector<CharacterListener*> listeners;
};

#endif /* CharacterManager_hpp */
