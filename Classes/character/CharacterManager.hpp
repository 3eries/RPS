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

class CharacterManager {    
public:
    static CharacterManager* getInstance();
    ~CharacterManager();
    
private:
    CharacterManager();
    
public:
    void init(const std::string &json);
    
    Package getPackage(size_t i);
    
private:
    CC_SYNTHESIZE_READONLY(std::vector<Package>, packages, Packages);
};

#endif /* CharacterManager_hpp */
