//
//  CommonMenu.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 16..
//

#include "CommonMenu.hpp"

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

CommonMenu::CommonMenu() :
topMenu(nullptr),
bottomMenu(nullptr),
onClickTopMenuListener(nullptr),
onClickBottomMenuListener(nullptr) {
    
}

CommonMenu::~CommonMenu() {
}

bool CommonMenu::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    return true;
}

void CommonMenu::onEnter() {
    
    Node::onEnter();
    
    addPopupListener();
}

void CommonMenu::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void CommonMenu::onExit() {
    
    PopupManager::getInstance()->removeListener(this);
    
    Node::onExit();
}

/**
 * 팝업 리스너 등록
 */
void CommonMenu::addPopupListener() {
    
    auto listener = PopupListener::create();
    listener->setTarget(this);
    listener->onEvent = [=](Node *sender, PopupEventType eventType) {
        
        /*
        auto popup = dynamic_cast<BasePopup*>(sender);
        auto type = popup->getType();
        
        // 팝업 등/퇴장에 따른 우상단 버튼 설정
        switch( type ) {
            case BasePopup::Type::EXIT_APP:
            case BasePopup::Type::RANKING: {
            // case BasePopup::Type::SETTING: {
                // 등장, 닫기 버튼
                if( eventType == PopupEventType::ENTER_ACTION ) {
                    topMenu->setRightMenu(TopMenu::Tag::BACK);
                }
                // 퇴장, 설정 버튼
                else if( eventType == PopupEventType::EXIT_ACTION ) {
                    topMenu->setRightMenu(TopMenu::Tag::SETTING);
                }
                
            } break;
                
            case BasePopup::Type::PAUSE: {
                // 등장, 닫기 버튼
                if( eventType == PopupEventType::ENTER_ACTION ) {
                    topMenu->setRightMenu(TopMenu::Tag::BACK);
                }
                // 퇴장, 일시정지 버튼
                else if( eventType == PopupEventType::EXIT_ACTION ) {
                    topMenu->setRightMenu(TopMenu::Tag::PAUSE);
                }
                
            } break;
                
            default:
                break;
        }
        */
    };
    
    PopupManager::getInstance()->addListener(listener);
}

/**
 * 상단 메뉴 생성
 */
void CommonMenu::setTopMenu(Node *parent, int zOrder) {

    if( !topMenu ) {
        topMenu = TopMenu::create();
        parent->addChild(topMenu);
        
        topMenu->setOnClickListener([=](TopMenu::Tag tag) {
            this->onClickTopMenu(tag);
        });
    }
    
    topMenu->setLocalZOrder(zOrder);
}

/**
 * 하단 메뉴 생성
 */
void CommonMenu::setBottomMenu(Node *parent, int zOrder) {
    
    if( !bottomMenu ) {
        bottomMenu = BottomMenu::create();
        parent->addChild(bottomMenu);
        
        bottomMenu->setOnClickListener([=](BottomMenu::Tag tag) {
            this->onClickBottomMenu(tag);
        });
    }
    
    bottomMenu->setLocalZOrder(zOrder);
}

void CommonMenu::setVisible(bool visible) {
    
    Node::setVisible(visible);
    
    if( topMenu )       topMenu->setVisible(visible);
    if( bottomMenu )    bottomMenu->setVisible(visible);
}

/**
 * 터치 활성화 설정
 */
void CommonMenu::setTouchEnabled(bool enabled) {
    
    topMenu->setTouchEnabled(enabled);
    bottomMenu->setTouchEnabled(enabled);
}

/**
 * 로컬 랭킹 팝업 노출
 */
void CommonMenu::showRankingPopup(OnPopupEvent onEventListener) {
    
    bottomMenu->setRankingButton(BottomMenu::Tag::RANKING_WORLD);
    
    auto popup = RankingPopup::create();
    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
        
        if( onEventListener ) {
            onEventListener(sender, eventType);
        }
    });
    popup->setOnDismissListener([=](Node*) {
//        this->runTopMenuEnterAction(RankingPopup::SLIDE_OUT_DURATION * 0.75f);
//        this->setRankingButton(Tag::RANKING_LOCAL);
        
        bottomMenu->setRankingButton(BottomMenu::Tag::RANKING_LOCAL);
    });
    
    SceneManager::getInstance()->getScene()->addChild(popup, PopupZOrder::BOTTOM);
    
    // 연출
    popup->runEnterAction();
}

/**
 * 설정 팝업 노출
 */
void CommonMenu::showSettingPopup() {
    
    auto popup = SettingPopup::create();
    popup->setOnClickMenuListener([=](SettingPopup::Tag tag) {
        
        switch( tag ) {
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
    SceneManager::getScene()->addChild(popup, PopupZOrder::MIDDLE);
}

/**
 * 메뉴 오픈
 */
void CommonMenu::openMenu() {
    
    topMenu->openMenu();
    bottomMenu->openMenu();
}

/**
 * 메뉴 닫기
 */
void CommonMenu::closeMenu() {
    
    topMenu->closeMenu();
    bottomMenu->closeMenu();
}

/**
 * 상단 메뉴 클릭
 */
void CommonMenu::onClickTopMenu(TopMenu::Tag tag) {
    
    if( onClickTopMenuListener && onClickTopMenuListener(tag) ) {
        return;
    }
    
    switch( tag ) {
        // 설정
        case TopMenu::Tag::SETTING: {
            showSettingPopup();
        } break;

        // 뒤로 가기
        case TopMenu::Tag::BACK: {
            auto popup = PopupManager::getInstance()->getFrontPopup();
            popup->dismissWithAction();
        } break;
            
        // 닫기
        case TopMenu::Tag::CLOSE: {
            auto popup = PopupManager::getInstance()->getFrontPopup();
            popup->dismissWithAction();
        } break;
            
        // 게임 일시정지
        case TopMenu::Tag::PAUSE: {
            
        } break;
            
        default:
            CCASSERT(false, "CommonMenu::onClickTopMenu error: invalid tag.");
            break;
    }
}

/**
 * 하단 메뉴 클릭
 */
void CommonMenu::onClickBottomMenu(BottomMenu::Tag tag) {
    
    if( onClickBottomMenuListener && onClickBottomMenuListener(tag) ) {
        return;
    }
    
    switch( tag ) {
        // 게임 시작
        case BottomMenu::Tag::START: {
            SceneManager::getInstance()->replace(SceneType::GAME);
        } break;
            
        // 로컬 랭킹
        case BottomMenu::Tag::RANKING_LOCAL: {
            showRankingPopup();
        } break;
            
        // 월드 랭킹
        case BottomMenu::Tag::RANKING_WORLD: {
        } break;
            
        // 상점
        case BottomMenu::Tag::SHOP: {
        } break;
            
        default:
            CCASSERT(false, "CommonMenu::onClickBottomMenu error: invalid tag.");
            break;
    }
}


