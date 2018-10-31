//
//  GetCharacterPopup.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#ifndef GetCharacterPopup_hpp
#define GetCharacterPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"
#include "CharacterManager.hpp"

/** @class GetCharacterPopup
 * @brief 캐릭터 획득 팝업
 */
class GetCharacterPopup : public BasePopup {
public:
    static GetCharacterPopup* create(Character character);
    ~GetCharacterPopup();
    
private:
    GetCharacterPopup(Character character);
    
    bool init() override;
    void onEnter() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
public:
    void runEnterAction(SBCallback onFinished = nullptr) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    Character character;
    
    cocos2d::Node *stoneBg;
};

#endif /* GetCharacterPopup_hpp */
