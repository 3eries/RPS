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
    void onGameModeChanged(GameMode mode) override;
    
public:
    void switchButton(GameMode mode);
    void setButtonTouchEnabled(bool enabled);
    
    void showTapHint(RSPType winHand);
    void hideTapHint();
    
private:
    CC_SYNTHESIZE(std::function<void(RSPType)>,
                  onNormalButtonClickListener, OnNormalButtonClickListener);
    CC_SYNTHESIZE(std::function<void(int)>,
                  onFeverButtonClickListener, OnFeverButtonClickListener);
    
    std::map<GameMode, cocos2d::Node*> buttonLayers;
    std::vector<RSPButton*> buttons;
    
    cocos2d::Node *tapHint;
};

#endif /* RSPButtonLayer_hpp */
