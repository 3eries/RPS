//
//  GameOverPopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameOverPopup_hpp
#define GameOverPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class GameOverPopup : public BasePopup {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    static const float FADE_IN_DURATION;
    static const float FADE_OUT_DURATION;
    
public:
    static GameOverPopup* create(int score);
    ~GameOverPopup();
    
private:
    GameOverPopup(int score);
    
    bool init() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    int score;
    
    cocos2d::Node *stone;       // 배경 비석
    cocos2d::Label *scoreLabel; // 스코어 라벨
    
    std::vector<cocos2d::Node*> fadeNodes;
};

#endif /* GameOverPopup_hpp */
