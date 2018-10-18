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
    CC_SYNTHESIZE_READONLY(std::vector<Package>, packages, Packages);
    std::map<std::string, Character> characters;
    
    std::map<std::string, PackageDB*> packageDbs;
};

#endif /* CharacterManager_hpp */
