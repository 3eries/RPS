//
//  CommonMenu.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#ifndef CommonMenu_hpp
#define CommonMenu_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "PopupListener.hpp"

#include "TopMenu.hpp"
#include "BottomMenu.hpp"

/** @class CommonMenu
 * @brief 공통 상,하단 메뉴를 정의합니다
 *
 */
class CommonMenu : public cocos2d::Node {
public:
    CREATE_FUNC(CommonMenu);
    ~CommonMenu();
    
private:
    CommonMenu();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void addPopupListener();
    
    void onClickTopMenu(TopMenu::Tag tag);
    void onClickBottomMenu(BottomMenu::Tag tag);
    
public:
    void setTopMenu(cocos2d::Node *parent, int zOrder = 0);
    void setBottomMenu(cocos2d::Node *parent, int zOrder = 0);
    
    void setVisible(bool visible) override;
    void setTouchEnabled(bool enabled);
    
    void showRankingPopup(OnPopupEvent onEventListener = nullptr);
    void showSettingPopup();
    
    void openMenu();
    void closeMenu();
    
private:
    CC_SYNTHESIZE_READONLY(TopMenu*, topMenu, TopMenu);
    CC_SYNTHESIZE_READONLY(BottomMenu*, bottomMenu, BottomMenu);
    
    CC_SYNTHESIZE(std::function<bool(TopMenu::Tag)>,
                  onClickTopMenuListener, OnClickTopMenuListener);
    CC_SYNTHESIZE(std::function<bool(BottomMenu::Tag)>,
                  onClickBottomMenuListener, OnClickBottomMenuListener);
};

#endif /* CommonMenu_hpp */
