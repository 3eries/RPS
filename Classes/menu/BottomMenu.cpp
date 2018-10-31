//
//  BottomMenu.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#include "BottomMenu.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

const float BottomMenu::SLIDE_IN_DURATION          = EffectDuration::MENU_SLIDE_NORMAL;
const float BottomMenu::SLIDE_OUT_DURATION         = EffectDuration::MENU_SLIDE_NORMAL;

static const Vec2       MENU_IN_POSITION           = Vec2(0, 0);
static const Vec2       MENU_OUT_POSITION          = Vec2(0, -180);

BottomMenu::BottomMenu() :
onClickListener(nullptr),
isOpened(false) {
    
}

BottomMenu::~BottomMenu() {
    
}

bool BottomMenu::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    initMenu();
    
    setTouchEnabled(true);
    setRankingButton(Tag::RANKING_LOCAL);
    
    return true;
}

void BottomMenu::onEnter() {
    
    Node::onEnter();
}

void BottomMenu::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void BottomMenu::onExit() {
    
    Node::onExit();
}

void BottomMenu::setTouchEnabled(bool enabled) {
    
    touchLockNode->setVisible(!enabled);
}

/**
 * 랭킹 버튼 설정 (로컬 or 월드)
 */
void BottomMenu::setRankingButton(Tag tag) {
    
    CCASSERT(tag == Tag::RANKING_LOCAL || tag == Tag::RANKING_WORLD, "BottomMenu::setRankingButton error: invalid tag.");
    
    // local
    if( tag == Tag::RANKING_LOCAL ) {
        getChildByTag(Tag::RANKING_LOCAL)->setVisible(true);
        getChildByTag(Tag::RANKING_WORLD)->setVisible(false);
    }
    // world
    else {
        getChildByTag(Tag::RANKING_LOCAL)->setVisible(false);
        getChildByTag(Tag::RANKING_WORLD)->setVisible(true);
    }
}

/**
 * 메뉴 오픈
 */
void BottomMenu::openMenu(float duration) {

    /*
    if( isOpened ) {
        return;
    }
    */
    
    isOpened = true;
    
    stopAllActions();
    setVisible(true);
    setTouchEnabled(false);
    
    auto onActionFinished = [=]() {
        this->setTouchEnabled(true);
    };
    
    if( duration == 0 ) {
        setPosition(MENU_IN_POSITION);
        onActionFinished();
        return;
    }
    
    // slide in
    SBActionHelper::runMoveAction(this, MENU_OUT_POSITION, MENU_IN_POSITION, duration);
    SBDirector::postDelayed(this, onActionFinished, duration);
}

/**
 * 메뉴 닫기
 */
void BottomMenu::closeMenu(float duration) {
    
    /*
    if( !isOpened ) {
        return;
    }
     */
    
    isOpened = false;
    
    stopAllActions();
    setVisible(true);
    setTouchEnabled(false);
    
    auto onActionFinished = [=]() {
        this->setVisible(false);
    };
    
    if( duration == 0 ) {
        setPosition(MENU_OUT_POSITION);
        onActionFinished();
        return;
    }
    
    // slide out
    SBActionHelper::runMoveAction(this, MENU_IN_POSITION, MENU_OUT_POSITION, duration);
    SBDirector::postDelayed(this, onActionFinished, duration);
}

/**
 * 버튼 클릭
 */
void BottomMenu::onClick(Node *sender) {
    
    const Tag tag = (Tag)sender->getTag();
    
    if( onClickListener ) {
        onClickListener(tag);
    }
}

/**
 * 배경 초기화
 */
void BottomMenu::initBg() {
    
    // 터치 잠금 노드
    touchLockNode = SBNodeUtils::createTouchNode();
    touchLockNode->setAnchorPoint(ANCHOR_MB);
    touchLockNode->setPosition(Vec2BC(0, 0));
    touchLockNode->setContentSize(Size(SB_WIN_SIZE.width, 200));
    addChild(touchLockNode, SBZOrder::TOP);
    
    // touchLockNode->addChild(SBNodeUtils::createBackgroundNode(touchLockNode, Color4B(255,0,0,255*0.5f)));
    
}

/**
 * 메뉴 초기화
 */
void BottomMenu::initMenu() {
    
    vector<SBUIInfo> infos({
        SBUIInfo(Tag::START,            ANCHOR_M,    Vec2BC(0,                 100),    "RSP_btn_start.png"),
        SBUIInfo(Tag::RANKING_LOCAL,    ANCHOR_ML,   Vec2BL(120 -  (216*0.5f), 100),    "RSP_btn_ranking.png"),
        SBUIInfo(Tag::RANKING_WORLD,    ANCHOR_ML,   Vec2BL(120 -  (216*0.5f), 100),    "RSP_btn_ranking_world.png"),
        SBUIInfo(Tag::SHOP,             ANCHOR_MR,   Vec2BR(-120 + (216*0.5f), 100),    "RSP_btn_shop.png"),
    });
    
    if( SBDirector::isPadResolution() ) {
        auto &worldRankingInfo = infos[2];
        worldRankingInfo.file = "RSP_btn_ranking_world_bottom.png";
    }
    
    for( int i = 0; i < infos.size(); ++i ) {
        auto info = infos[i];
        string file = DIR_IMG_GAME + info.file;
        
        if( SBDirector::isPadResolution() ) {
            file = SBStringUtils::replaceAll(file, DIR_IMG_GAME, DIR_IMG_GAME_IPAD);
        }
        
        auto btn = SBButton::create(file);
        btn->setZoomScale(0.05f);
        info.apply(btn);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(BottomMenu::onClick, this));
    }
    
    // START 버튼 연출
    /*
     {
     auto scale1 = ScaleTo::create(0.005f, 1.07f, 0.91f);
     auto scale2 = ScaleTo::create(0.005f, 1.15f, 1.13f);
     auto scale3 = ScaleTo::create(0.005f, 1.09f, 1.27f);
     auto scale4 = ScaleTo::create(0.065f, 0.95f, 1.06f);
     auto scale5 = ScaleTo::create(0.065f, 1.0f, 1.0f);
     auto scaleSeq = Sequence::create(scale1, scale2, scale3, scale4, scale5, nullptr);
     
     auto delay = DelayTime::create(5.0f);
     auto seq = Sequence::create(delay, scaleSeq, nullptr);
     auto repeat = RepeatForever::create(seq);
     
     auto btn = getChildByTag<SBButton*>(Tag::BTN_START);
     auto contentLayer = btn->getContentsLayer();
     
     btn->setOnTouchListener([=](Node*, SBTouchEventType eventType) {
     
     switch( eventType ) {
     case SBTouchEventType::BEGAN: {
     contentLayer->pause();
     } break;
     
     case SBTouchEventType::ENDED:
     case SBTouchEventType::CANCELED: {
     contentLayer->resume();
     } break;
     
     default:
     break;
     }
     });
     
     contentLayer->runAction(repeat);
     }
     */
}

