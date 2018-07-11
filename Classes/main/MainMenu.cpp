//
//  MainMenu.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 3..
//

#include "MainMenu.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "UIHelper.hpp"

#include "SettingPopup.hpp"
#include "RankingPopup.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

const float MainMenu::SLIDE_IN_DURATION          = 0.15f;
const float MainMenu::SLIDE_OUT_DURATION         = 0.15f;

static const Vec2  TOP_MENU_IN_POSITION          = Vec2(0, 0);
static const Vec2  TOP_MENU_OUT_POSITION         = Vec2(110, 0);

static const Vec2  BOTTOM_MENU_IN_POSITION       = Vec2(0, 0);
static const Vec2  BOTTOM_MENU_OUT_POSITION      = Vec2(0, -180);

MainMenu::MainMenu() :
onClickListener(nullptr) {
    
}

MainMenu::~MainMenu() {
    
}

bool MainMenu::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    initTopMenu();
    initBottomMenu();
    
    setTouchEnabled(true);
    setRankingButton(Tag::RANKING_LOCAL);
    
    return true;
}

void MainMenu::onEnter() {
    
    Node::onEnter();
}

void MainMenu::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void MainMenu::onExit() {
    
    Node::onExit();
}

void MainMenu::setTouchEnabled(bool enabled) {
    
    touchLockNode->setVisible(!enabled);
}

void MainMenu::setRankingButton(Tag tag) {
    
    CCASSERT(tag == Tag::RANKING_LOCAL || tag == Tag::RANKING_WORLD, "MainMenu::setRankingButton error: invalid tag.");
    
    // local
    if( tag == Tag::RANKING_LOCAL ) {
        bottomMenu->getChildByTag(Tag::RANKING_LOCAL)->setVisible(true);
        bottomMenu->getChildByTag(Tag::RANKING_WORLD)->setVisible(false);
    }
    // world
    else {
        bottomMenu->getChildByTag(Tag::RANKING_LOCAL)->setVisible(false);
        bottomMenu->getChildByTag(Tag::RANKING_WORLD)->setVisible(true);
    }
}

/**
 * 로컬 랭킹 팝업 노출
 */
void MainMenu::showRankingPopup() {
    
    setRankingButton(Tag::RANKING_WORLD);
    
    auto popup = RankingPopup::create();
    SceneManager::getInstance()->getScene()->addChild(popup, POPUP_ZORDER);
    
    popup->setOnDismissListener([=](Node*) {
        this->runTopMenuEnterAction(RankingPopup::SLIDE_OUT_DURATION * 0.75f);
        this->setRankingButton(Tag::RANKING_LOCAL);
    });
    
    // 연출
    popup->runEnterAction();
    runTopMenuExitAction(RankingPopup::SLIDE_IN_DURATION * 0.75);
}

/**
 * 설정 팝업 노출
 */
void MainMenu::showSettingPopup() {
 
    auto popup = SettingPopup::create();
    popup->setOnClickMenuListener([=](SettingPopup::Tag tag) {
        
        switch( tag ) {
            // close
            case SettingPopup::Tag::CLOSE: {
                popup->dismissWithAction();
            } break;

            // world ranking
            case SettingPopup::Tag::WORLD_RANKING: {
                // TODO:
                // popup->dismissWithAction();
            } break;
                
            // restore purchase
            case SettingPopup::Tag::RESTORE_PURCHASE: {
                // TODO:
                // popup->dismissWithAction();
            } break;
                
            // remove ads
            case SettingPopup::Tag::REMOVE_ADS: {
                // TODO:
                // popup->dismissWithAction();
            } break;
                
            default: break;
        }
    });
    SceneManager::getScene()->addChild(popup, SBZOrder::TOP);
}

/**
 * 메뉴 오픈
 */
void MainMenu::openMenu() {
    
    /*
    if( topMenu->isOpened() && bottomMenu->isOpened() ) {
        // 이미 오픈됨
        return;
    }
     */
    
    stopAllActions();
    setVisible(true);
    setTouchEnabled(false);
    
    if( !topMenu->isOpened() )      runTopMenuEnterAction();
    if( !bottomMenu->isOpened() )   runBottomMenuEnterAction();
    
    topMenu->setOpened(true);
    bottomMenu->setOpened(true);
    
    auto delay = DelayTime::create(SLIDE_IN_DURATION);
    auto callFunc = CallFunc::create([=]() {
        this->setTouchEnabled(true);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 메뉴 닫기
 */
void MainMenu::closeMenu() {
    
    /*
    if( !topMenu->isOpened() && !bottomMenu->isOpened() ) {
        // 이미 닫힘
        return;
    }
    */
    
    stopAllActions();
    setVisible(true);
    setTouchEnabled(false);
    
    if( topMenu->isOpened() )      runTopMenuExitAction();
    if( bottomMenu->isOpened() )   runBottomMenuExitAction();
    
    topMenu->setOpened(false);
    bottomMenu->setOpened(false);
    
    auto delay = DelayTime::create(SLIDE_OUT_DURATION);
    auto callFunc = CallFunc::create([=]() {
        this->setVisible(false);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * Top Menu 등장 액션
 */
void MainMenu::runTopMenuEnterAction(float duration) {
    CCLOG("MainMenu::runTopMenuEnterAction: %f", duration);
    runMenuMoveAction(topMenu, TOP_MENU_OUT_POSITION, TOP_MENU_IN_POSITION, duration);
}

/**
 * Top Menu 퇴장 액션
 */
void MainMenu::runTopMenuExitAction(float duration) {
    CCLOG("MainMenu::runTopMenuExitAction: %f", duration);
    runMenuMoveAction(topMenu, TOP_MENU_IN_POSITION, TOP_MENU_OUT_POSITION, duration);
}

/**
 * Bottom Menu 등장 액션
 */
void MainMenu::runBottomMenuEnterAction() {
    runMenuMoveAction(bottomMenu, BOTTOM_MENU_OUT_POSITION, BOTTOM_MENU_IN_POSITION, SLIDE_IN_DURATION);
}

/**
 * Bottom Menu 퇴장 액션
 */
void MainMenu::runBottomMenuExitAction() {
    runMenuMoveAction(bottomMenu, BOTTOM_MENU_IN_POSITION, BOTTOM_MENU_OUT_POSITION, SLIDE_OUT_DURATION);
}

void MainMenu::runMenuMoveAction(Node *menu, Vec2 from, Vec2 to, float duration) {
    
    menu->stopAllActions();
    
    menu->setPosition(from);
    menu->runAction(MoveTo::create(duration, to));
}

/**
 * 버튼 클릭
 */
void MainMenu::onClick(Node *sender) {
    
    const Tag tag = (Tag)sender->getTag();
    
    if( onClickListener && onClickListener(tag) ) {
        return;
    }
    
    switch( tag ) {
        // 게임 시작
        case Tag::START: {
            SceneManager::getInstance()->replace(SceneType::GAME);
        } break;
            
        // 로컬 랭킹
        case Tag::RANKING_LOCAL: {
            showRankingPopup();
        } break;
            
        // 월드 랭킹
        case Tag::RANKING_WORLD: {
        } break;
            
        // 상점
        case Tag::SHOP: {
        } break;
            
        // 설정
        case Tag::SETTING: {
            showSettingPopup();
        } break;
            
        default: break;
    }
}

/**
 * 배경 초기화
 */
void MainMenu::initBg() {
    
    // 터치 잠금 노드
    touchLockNode = SBNodeUtils::createTouchNode();
    addChild(touchLockNode, SBZOrder::TOP);
}

/**
 * 상단 메뉴 초기화
 */
void MainMenu::initTopMenu() {
    
    topMenu = Menu::create();
    SBNodeUtils::createZeroSizeNode(topMenu);
    addChild(topMenu);
    
    // RSP_btn_option.png Vec2TC(298, -158) , Size(92, 92)   // 배너 좌표 고려?
    // RSP_btn_option.png Vec2TC(298, -56) , Size(92, 92)
    vector<SBUIInfo> infos = {
        // SBUIInfo(Tag::SETTING,       ANCHOR_TR,   Vec2TR(-10, -10),  "RSP_btn_option.png"),
        SBUIInfo(Tag::SETTING,       ANCHOR_M,   Vec2TC(298, -56),  "RSP_btn_option.png"),
    };
    
    if( SceneManager::isBannerVisible() ) {
        auto &settingInfo = infos[0];
        settingInfo.pos = Vec2TC(298, -158);
    }
    
    for( int i = 0; i < infos.size(); ++i ) {
        auto info = infos[i];
        
        auto btn = SBButton::create(DIR_IMG_GAME + info.file);
        btn->setZoomScale(0.1f);
        info.apply(btn);
        topMenu->addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(MainMenu::onClick, this));
    }
    
    CCLOG("top menu size: (%f, %f)",
          SBNodeUtils::getChildrenBoundingSize(topMenu).width,
          SBNodeUtils::getChildrenBoundingSize(topMenu).height);
}

/**
 * 하단 메뉴 초기화
 */
void MainMenu::initBottomMenu() {
    
    bottomMenu = Menu::create();
    SBNodeUtils::createZeroSizeNode(bottomMenu);
    addChild(bottomMenu);
    
    // RSP_btn_ranking.png Vec2BC(-268, 100) , Size(160, 152)
    // RSP_btn_start.png Vec2BC(0, 100) , Size(352, 152)
    // RSP_btn_shop.png Vec2BC(268, 100) , Size(160, 152)
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
        bottomMenu->addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(MainMenu::onClick, this));
    }
    
    // Shop 버튼 커밍순
    {
        auto btn = bottomMenu->getChildByTag<SBButton*>(Tag::SHOP);
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

