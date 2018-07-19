//
//  TopMenu.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#ifndef TopMenu_hpp
#define TopMenu_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "PopupListener.hpp"

#include "TopMenu.hpp"
#include "BottomMenu.hpp"

/** @class TopMenu
 * @brief 공통 상단 메뉴를 정의합니다
 *
 */
class TopMenu : public cocos2d::Node, public SBNodeListener {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    
    enum Tag {
        NONE = -1,
        
        SETTING     = 100,  // 설정
        BACK,               // 뒤로 가기
        CLOSE,              // 닫기
        PAUSE,              // 게임 일시정지
    };
    
public:
    CREATE_FUNC(TopMenu);
    ~TopMenu();
    
private:
    TopMenu();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initMenu();
    
    void onClick(cocos2d::Node *sender) override;
    
public:
    void setTouchEnabled(bool enabled);
    
    void openMenu(float duration = SLIDE_IN_DURATION);
    void closeMenu(float duration = SLIDE_OUT_DURATION);
    
    void setRightMenu(Tag tag, float actionDuration);
    void setRightMenu(Tag tag);
    
private:
    CC_SYNTHESIZE(std::function<void(Tag)>, onClickListener, OnClickListener);
    
    cocos2d::Node *touchLockNode;
    
    bool isOpened;
    
    std::vector<cocos2d::Node*> menus;
    Tag selectedRightMenu;
};

#endif /* TopMenu_hpp */
