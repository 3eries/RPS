//
//  PlayGuidePopup.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 29/11/2018.
//

#ifndef PlayGuidePopup_hpp
#define PlayGuidePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

/** @class PlayGuidePopup
 * @brief 게임 플레이 가이드 팝업
 */
class PlayGuidePopup : public BasePopup {
public:
    CREATE_FUNC(PlayGuidePopup);
    ~PlayGuidePopup();
    
private:
    PlayGuidePopup();
    
    bool init() override;
    void onEnter() override;
    
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void onEnterActionFinished() override;
};

#endif /* PlayGuidePopup_hpp */
