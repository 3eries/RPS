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
#include "../CharacterListener.hpp"

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
        DRAW,           // 비긴 횟수
        CREDIT,         // 크레딧 진입 횟수
    };
    
public:
    PackageDB(Package pack);
    ~PackageDB();
    
    std::string toString();
    
public:
    void            commit();
    
    Characters      getLockedCharacters();
    
    bool            isPackageUnlocked();
    bool            isCharacterUnlocked(const std::string &charId);
    
    int             getFieldValue(Field f);
    int             getViewAdsValue(const std::string &charId);
    int             getUnlockFieldValue(Character c);
    
    void            unlockPackage();
    void            unlockCharacter(const std::string &charId);
    
    void            submit(OnCharacterListListener listener, Field field,
                           int i = 1, const std::string &charId = "");
    
private:
    CC_SYNTHESIZE_READONLY(Package, pack, Package);   // 패키지 기본 데이터
    std::map<Field, int> fieldValues;                 // DB 필드 값
    
    // Key : 캐릭터 아이디, Value : 광고 시청 횟수
    std::map<std::string, int> viewAdsValues;
    
    std::vector<std::string> unlockCharacters;        // 잠금 해제된 캐릭터 리스트
};

#endif /* PackageDB_hpp */
