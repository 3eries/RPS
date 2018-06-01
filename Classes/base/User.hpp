//
//  User.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef User_hpp
#define User_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class User {
public:
    static User* getInstance();
    static void destroyInstance();
    
    ~User();
    
private:
    User();
    
public:
    void init();
    
    static bool isOwnRemoveAdsItem();
    static void setOwnRemoveAdsItem(bool owned);
    static void removeAds();
};

#endif /* User_hpp */
