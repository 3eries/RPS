//
//  PackageDB.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 16..
//

#ifndef PackageDB_hpp
#define PackageDB_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "../model/Package.h"

/** @class PackageDB
 * @brief 1개 캐릭터 패키지의 DB
 */
class PackageDB : public cocos2d::Ref {
public:
    enum class Field {
        NONE = 0,
        BEST_SCORE,     // 최고 스코어
        TOTAL_SCORE,    // 달성한 스코어의 총 합
        GAME_PLAY,      // 플레이한 게임의 판 수
        GAME_OVER,      // 게임 오버한 횟수
        DAILY_LOGIN,    // 게임에 접속한 횟수
        FEVER,          // 피버 진입 횟수
        VIEW_ADS,       // 동영상 광고 시청 횟수
    };
    
    typedef std::function<void(std::vector<std::string>)> OnCharacterUnlocked;
    
public:
    PackageDB(Package pack);
    ~PackageDB();
    
    std::string toString();
    
public:
    void    commit();
    
    bool    isPackageUnlocked();
    bool    isCharacterUnlocked(const std::string &charId);
    int     getValue(Field field);
    
    void    unlockPackage();
    void    unlockCharacter(const std::string &charId);
    
    void    submit(Field field, int i = 1,
                   OnCharacterUnlocked onCharacterUnlocked = nullptr);
    
private:
    Package pack;
    bool isUnlocked;
    std::map<Field, int> fieldValues;
    std::vector<std::string> unlockCharacters;
};

#endif /* PackageDB_hpp */
