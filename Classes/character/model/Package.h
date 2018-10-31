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
    std::string name;
    std::vector<Character> characters;
    
    Package() {
        packId = "";
    }
    
    std::string toString() {
        std::string str = "Package {\n";
        str += STR_FORMAT("\tpackId: %s\n", packId.c_str());
        str += STR_FORMAT("\tname: %s\n", name.c_str());
        
        for( Character c : characters ) {
            str += c.toString() + "\n";
        }
        
        str += "}";
        
        return str;
    }
};

typedef std::vector<Package>                 Packages;
typedef std::vector<std::string>             PackageIds;

typedef std::function<void(Package)>         OnPackageListener;
typedef std::function<void(Packages)>        OnPackageListListener;
typedef std::function<void(PackageIds)>      OnPackageIdListListener;

static inline PackageIds getPackageIds(Packages packs) {
    
    PackageIds ids;
    
    for( auto pack : packs ) {
        ids.push_back(pack.packId);
    }
    
    return ids;
}

static inline std::string packIdToString(PackageIds ids) {
    
    std::string str = "";
    
    for( auto packId : ids ) {
        str += packId + ", ";
    }
    
    return str;
}

#endif /* Package_h */
