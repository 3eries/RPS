//
//  TopMenu.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#include "TopMenu.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

const float TopMenu::SLIDE_IN_DURATION         = EffectDuration::MENU_SLIDE_NORMAL;
const float TopMenu::SLIDE_OUT_DURATION        = EffectDuration::MENU_SLIDE_NORMAL;

static const float   RIGHT_MENU_SLIDE_DURATION = EffectDuration::MENU_SLIDE_FAST;

static const Vec2    MENU_IN_POSITION          = Vec2(0, 0);
static const Vec2    MENU_OUT_POSITION         = Vec2(110, 0);

#define              MENU_BUTTON_POSITION \
(Vec2TC(295, -62) + (SceneManager::isBannerVisible() ? Vec2(0, -BANNER_HEIGHT) : Vec2(0, 0)))

TopMenu::TopMenu() :
onClickListener(nullptr),
isOpened(false),
selectedRightMenu(Tag::NONE) {
    
}

TopMenu::~TopMenu() {
    
}

bool TopMenu::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    initMenu();
    
    setTouchEnabled(true);
    setRightMenu(Tag::SETTING, 0);
    
    return true;
}

void TopMenu::onEnter() {
    
    Node::onEnter();
}

void TopMenu::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void TopMenu::onExit() {
    
    Node::onExit();
}

void TopMenu::setTouchEnabled(bool enabled) {
    
    touchLockNode->setVisible(!enabled);
}

/**
 * 메뉴 오픈
 */
void TopMenu::openMenu(float duration) {
    
    /*
     if( isOpened ) {
     return;
     }
     */
    isOpened = true;
    
    stopAllActions();
    setVisible(true);
    setTouchEnabled(false);
    
    // slide in
    SBActionHelper::runMoveAction(this, MENU_OUT_POSITION, MENU_IN_POSITION, duration);
    
    // callback
    auto delay = DelayTime::create(duration);
    auto callFunc = CallFunc::create([=]() {
        this->setTouchEnabled(true);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 메뉴 닫기
 */
void TopMenu::closeMenu(float duration) {
    
    /*
     if( !isOpened ) {
     return;
     }
     */
    
    isOpened = false;
    
    stopAllActions();
    setVisible(true);
    setTouchEnabled(false);
    
    // slide out
    SBActionHelper::runMoveAction(this, MENU_IN_POSITION, MENU_OUT_POSITION, duration);
    
    // callback
    auto delay = DelayTime::create(duration);
    auto callFunc = CallFunc::create([=]() {
        this->setVisible(false);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 우측 메뉴 설정 (설정 or 닫기)
 */
void TopMenu::setRightMenu(Tag tag, float actionDuration) {
    
    if( selectedRightMenu == tag ) {
        return;
    }
    
    auto prevTag = selectedRightMenu;
    selectedRightMenu = tag;
    
    if( prevTag == Tag::NONE ) {
        getChildByTag(tag)->setVisible(true);
        return;
    }
    
    auto prevMenu = getChildByTag(prevTag);
    auto selectedMenu = getChildByTag(tag);
    
    prevMenu->stopAllActions();
    selectedMenu->stopAllActions();
    
    // action
    if( actionDuration > 0 ) {
        actionDuration *= 0.5f;
        
        const Vec2 IN_POS = MENU_BUTTON_POSITION;
        const Vec2 OUT_POS = Vec2(SB_WIN_SIZE.width + selectedMenu->getContentSize().width*0.5f,
                                  MENU_BUTTON_POSITION.y);
        
        // Step 1. 기존 메뉴 퇴장
        auto move = MoveTo::create(actionDuration, OUT_POS);
        auto callFunc = CallFunc::create([=]() {
           
            // Step 2. 새 메뉴 등장
            selectedMenu->setVisible(true);
            selectedMenu->setPosition(OUT_POS);
            selectedMenu->runAction(MoveTo::create(actionDuration, IN_POS));
        });
        prevMenu->runAction(Sequence::create(move, callFunc, Hide::create(), nullptr));
        
    }
    // no action
    else {
        prevMenu->setVisible(false);
        selectedMenu->setVisible(true);
    }
}

void TopMenu::setRightMenu(Tag tag) {
    
    setRightMenu(tag, RIGHT_MENU_SLIDE_DURATION);
}

/**
 * 버튼 클릭
 */
void TopMenu::onClick(Node *sender) {
    
    const Tag tag = (Tag)sender->getTag();
    
    if( onClickListener ) {
        onClickListener(tag);
    }
}

/**
 * 배경 초기화
 */
void TopMenu::initBg() {
    
    // 터치 잠금 노드
    touchLockNode = SBNodeUtils::createTouchNode();
    touchLockNode->setAnchorPoint(ANCHOR_MT);
    touchLockNode->setPosition(Vec2TC(0, 0));
    touchLockNode->setContentSize(Size(SB_WIN_SIZE.width, 140));
    addChild(touchLockNode, SBZOrder::TOP);
    
    // touchLockNode->addChild(SBNodeUtils::createBackgroundNode(touchLockNode, Color4B(0,0,255,255*0.5f)));
}

/**
 * 메뉴 초기화
 */
void TopMenu::initMenu() {
    
    // RSP_btn_go_back.png Vec2TC(295, -62) , Size(100, 92)
    // RSP_btn_close.png Vec2TC(295, -62) , Size(100, 92)
    // RSP_btn_option.png Vec2TC(295, -62) , Size(100, 92)
    SBUIInfo infos[] = {
        SBUIInfo(Tag::SETTING,    ANCHOR_M,   MENU_BUTTON_POSITION,     "RSP_btn_option.png"),
        SBUIInfo(Tag::BACK,       ANCHOR_M,   MENU_BUTTON_POSITION,     "RSP_btn_go_back.png"),
        SBUIInfo(Tag::CLOSE,      ANCHOR_M,   MENU_BUTTON_POSITION,     "RSP_btn_close.png"),
        SBUIInfo(Tag::PAUSE,      ANCHOR_M,   MENU_BUTTON_POSITION,     "RSP_btn_pause.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
     
        // 배너 유무에 따른 좌표 설정
//        if( SceneManager::isBannerVisible() ) {
//            info.pos.y -= BANNER_HEIGHT;
//        }
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        info.apply(btn);
        btn->setZoomScale(0.05f);
        btn->setVisible(false);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(TopMenu::onClick, this));
    }
}

