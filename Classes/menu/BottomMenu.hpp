//
//  BottomMenu.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#ifndef BottomMenu_hpp
#define BottomMenu_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "PopupListener.hpp"

/** @class BottomMenu
 * @brief 공통 하단 메뉴를 정의합니다
 *
 */
class BottomMenu : public cocos2d::Node, public SBNodeListener {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    
    enum Tag {
        START            = 100,     // 게임 시작
        RANKING_LOCAL,              // 로컬 랭킹
        RANKING_WORLD,              // 월드 랭킹
        SHOP,                       // 샵
    };
    
public:
    CREATE_FUNC(BottomMenu);
    ~BottomMenu();
    
private:
    BottomMenu();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    
    void initBg();
    void initMenu();
    
    void onClick(cocos2d::Node *sender) override;
    
public:
    void setTouchEnabled(bool enabled);
    void setRankingButton(Tag tag);
    
    void openMenu();
    void closeMenu();
    
private:
    CC_SYNTHESIZE(std::function<void(Tag)>, onClickListener, OnClickListener);
    
    cocos2d::Node *touchLockNode;
    bool isOpened;
};

#endif /* BottomMenu_hpp */
