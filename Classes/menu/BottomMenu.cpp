//
//  BottomMenu.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#include "BottomMenu.hpp"

#include "RSP.h"
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
void BottomMenu::openMenu() {

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
    SBActionHelper::runMoveAction(this, MENU_OUT_POSITION, MENU_IN_POSITION, SLIDE_IN_DURATION);
    
    // callback
    auto delay = DelayTime::create(SLIDE_IN_DURATION);
    auto callFunc = CallFunc::create([=]() {
        this->setTouchEnabled(true);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 메뉴 닫기
 */
void BottomMenu::closeMenu() {
    
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
    SBActionHelper::runMoveAction(this, MENU_IN_POSITION, MENU_OUT_POSITION, SLIDE_OUT_DURATION);
    
    // callback
    auto delay = DelayTime::create(SLIDE_OUT_DURATION);
    auto callFunc = CallFunc::create([=]() {
        this->setVisible(false);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
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
    
    SBUIInfo infos[] = {
        SBUIInfo(Tag::START,                  ANCHOR_M,   Vec2BC(0,    100),     "RSP_btn_start.png"),
        SBUIInfo(Tag::RANKING_LOCAL,          ANCHOR_M,   Vec2BC(-268, 100),     "RSP_btn_ranking.png"),
        SBUIInfo(Tag::RANKING_WORLD,          ANCHOR_M,   Vec2BC(-268, 100),     "RSP_btn_ranking_world.png"),
        SBUIInfo(Tag::SHOP,                   ANCHOR_M,   Vec2BC(268,  100),     "RSP_btn_shop.png"),
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        btn->setZoomScale(0.05f);
        info.apply(btn);
        addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(BottomMenu::onClick, this));
    }
    
    // Shop 버튼 커밍순
    {
        auto btn = getChildByTag<SBButton*>(Tag::SHOP);
        btn->setTouchEnabled(false);
        
        // auto btnBox = SBNodeUtils::getBoundingBoxInWorld(btn);
        // comingSoon->setPosition(Vec2(btnBox.getMidX(), btnBox.getMidY()));
        
        auto comingSoon = SkeletonAnimation::createWithJsonFile(DIR_ANIM + "coming_soon.json");
        comingSoon->setAnchorPoint(Vec2::ZERO);
        comingSoon->setPosition(Vec2MC(btn->getContentSize(), 0, 0));
        comingSoon->setAnimation(0, ANIM_NAME_RUN, true);
        btn->addChild(comingSoon);
        
        comingSoon->setOpacity(0);
        comingSoon->runAction(FadeIn::create(1.0f));
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

