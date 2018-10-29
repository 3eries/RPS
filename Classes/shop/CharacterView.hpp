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
    void setCharacter(Character character);
    
    cocos2d::Node* getBackground();
    
private:
    Character character;
    
    cocos2d::Node *comingSoonView;
    cocos2d::Label *unlockDescLabel;
    std::vector<cocos2d::Label*> unlockAmountLabels;    // '진행도' '/' '조건'
    cocos2d::Sprite *characterTitleImage;
    SBAnimationSprite *characterImage;
};

#endif /* CharacterView_hpp */
