//
//  CharacterListener.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 26..
//

#ifndef CharacterListener_hpp
#define CharacterListener_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "model/Package.h"

class CharacterListener : public cocos2d::Ref {
public:
    SB_REF_CREATE_FUNC(CharacterListener);
    CharacterListener();
    ~CharacterListener();
    
    OnCharacterListener         onCharacterSelected;       // 캐릭터 선택됨
    
    OnPackageListListener       onPackageUnlocked;         // 패키지 잠금 해제
    OnCharacterListListener     onCharacterUnlocked;       // 캐릭터 잠금 해제

    OnPackageListListener       onPackageRestored;         // 패키지 IAP 복원
    OnCharacterListListener     onCharacterRestored;       // 캐릭터 IAP 복원
    
private:
    CC_SYNTHESIZE(cocos2d::Ref*, target, Target);
};

#endif /* CharacterListener_hpp */
