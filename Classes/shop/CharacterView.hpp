//
//  CharacterView.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#ifndef CharacterView_hpp
#define CharacterView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "CharacterManager.hpp"

class CharacterView : public cocos2d::Node {
private:
    enum Tag {
        BG = 1,
        COMING_SOON_VIEW,     // 커밍순 뷰
        UNLOCK_DESC,          // 잠금 해제 조건 설명
        CHAR_NAME,            // 캐릭터 이름
        CHAR_IMAGE,           // 캐릭터 이미지
    };
    
public:
    static CharacterView* create();
    ~CharacterView();
    
private:
    CharacterView();
    
    bool init() override;
    void onEnterTransitionDidFinish() override;
    
    void initBg();
    void initComingSoon();
    void initLabel();
    void initImage();
    
public:
    void updateSelf();
    void setCharacter(Character character);
    void runCharacterAnimation(bool isIdle);
    
    cocos2d::Node* getBackground();
    
private:
    Character character;

    // 잠금 해제 수량, '진행도' '/' '조건'
    std::vector<cocos2d::Label*> unlockAmountLabels;
};

#endif /* CharacterView_hpp */
