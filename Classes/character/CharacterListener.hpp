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
    ~CharacterListener();
    
    std::function<void(const std::vector<std::string>&)> onPackageUnlocked;         // 패키지 잠금 해제
    std::function<void(const std::vector<std::string>&)> onCharacterUnlocked;       // 캐릭터 잠금 해제
    
private:
    CharacterListener();
    
    CC_SYNTHESIZE(cocos2d::Ref*, target, Target);
};

#endif /* CharacterListener_hpp */
