//
//  Package.h
//  RPS
//
//  Created by seongmin hwang on 2018. 10. 11..
//

#ifndef Package_h
#define Package_h

#include "cocos2d.h"
#include "Character.h"

struct Package {
    std::string packId;
    std::vector<Character> characters;
    
    Package() {
        packId = "";
    }
    
    std::string toString() {
        std::string str = "Package {\n";
        str += STR_FORMAT("\tpackId: %s\n", packId.c_str());
        
        for( Character c : characters ) {
            str += c.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

#endif /* Package_h */
