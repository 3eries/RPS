//
//  RSPButtonLayer.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 5..
//

#ifndef RSPButtonLayer_hpp
#define RSPButtonLayer_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RSP.h"
#include "GameConfiguration.hpp"
#include "../GameManager.hpp"

class RSPButton;

class RSPButtonLayer : public cocos2d::Node, public GameListener {
public:
    CREATE_FUNC(RSPButtonLayer);
    ~RSPButtonLayer();
    
private:
    RSPButtonLayer();
    
private:
    bool init() override;
    void onExit() override;
    
    void initNormalButtons();
    void initFeverButtons();
    void initTapHint();
    
// GameListener
private:
    void onGameStart() override;
    void onPreGameOver() override;
    void onContinue() override;
    void onGameOver() override;
    void onGameModeChanged(GameMode mode) override;
    void onLastFeverBlockHit() override;
    
public:
    void switchButton(GameMode mode);
    void setButtonTouchEnabled(bool enabled);
    
    void touchLocked(float delay);
    
    void showTapHint(std::vector<RSPButton*> buttons);
    void showTapHint(RSPType winHand);
    void showTapHintFeverMode();
    
    void hideTapHint(bool runAction = true);
    
private:
    void onClickNormalButton(RSPType type);
    void onClickFeverButton(int i);
    
private:
    CC_SYNTHESIZE(std::function<void(RSPType)>,
                  onNormalButtonClickListener, OnNormalButtonClickListener);
    CC_SYNTHESIZE(std::function<void(int)>,
                  onFeverButtonClickListener, OnFeverButtonClickListener);
    
    std::map<GameMode, cocos2d::Node*> buttonLayers;
    
    static const int FEVER_MODE_BUTTON_INDEX = 3;
    std::vector<RSPButton*> buttons; // 노멀 모드 3개 + 피버 모드 2개
    
    cocos2d::Node *tapHintLayer;
    std::vector<cocos2d::Node*> tapHints;
    
    int feverButtonTouchCount;
};

#endif /* RSPButtonLayer_hpp */
