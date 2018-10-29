//
//  ShopPopup.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 22..
//

#include "ShopPopup.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"
#include "PopupManager.hpp"
#include "PackageNavigator.hpp"
#include "CharacterView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

const float ShopPopup::SLIDE_IN_DURATION               = EffectDuration::POPUP_SLIDE_NORMAL;
const float ShopPopup::SLIDE_OUT_DURATION              = EffectDuration::POPUP_SLIDE_NORMAL;

#define                SLIDE_IN_POSITION               Vec2(0, 0)
#define                SLIDE_OUT_POSITION              Vec2TL(0, -SBNodeUtils::getBoundingBoxInWorld(characterView->getBackground()).getMinY())

ShopPopup* ShopPopup::create() {
    
    auto popup = new ShopPopup();
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

ShopPopup::ShopPopup() : BasePopup(Type::SHOP),
charMgr(CharacterManager::getInstance()),
pack(charMgr->getPackage(charMgr->getPackageOrder()[0])),
characterIndex(0) {
    
}

ShopPopup::~ShopPopup() {
    
}

bool ShopPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ShopPopup::onEnter() {
    
    BasePopup::onEnter();
}

void ShopPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    initBottomMenu();
}

void ShopPopup::initContentView() {
    
    BasePopup::initContentView();
    
    initNavigator();
    initCharacter();
}

/**
 * 버튼 클릭
 */
void ShopPopup::onClick(Node *sender) {
    
    const Tag tag = (Tag)sender->getTag();
    
    switch( tag ) {
        case Tag::BTN_LEFT:                 setCharacter(characterIndex-1);        break;
        case Tag::BTN_RIGHT:                setCharacter(characterIndex+1);         break;
        case Tag::BTN_SELECT: {
            
        } break;
            
        case Tag::BTN_BUY: {
            
        } break;
            
        case Tag::BTN_VIEW_ADS: {
            
        } break;
            
        default: break;
    }
}

/**
 * 패키지 변경
 */
void ShopPopup::onPackageChanged(Package pack) {
    
    CCLOG("ShopPopup::onPackageChanged:\n%s", pack.toString().c_str());
    
    this->pack = pack;
    
    // 유효한 패키지
    if( pack.packId != "" ) {
        setCharacter(0);
        bottomMenu->setVisible(true);
    }
    // 커밍순 패키지
    else {
        characterIndex = 0;
        characterView->setCharacter(Character());
        bottomMenu->setVisible(false);
    }
}

/**
 * 캐릭터 변경
 */
void ShopPopup::setCharacter(int i) {
    
    characterIndex = i;
    
    // loop를 위한 인덱스 보정
    if( i < 0 )                               characterIndex = pack.characters.size()-1;
    else if( i > pack.characters.size()-1 )   characterIndex = 0;
    
    // 캐릭터 UI 변경
    characterView->setCharacter(pack.characters[characterIndex]);
}

/**
 * 현재 캐릭터 반환
 */
Character ShopPopup::getCharacter() {
    
    return pack.characters[characterIndex];
}

/**
 * 패키지 네비게이터 초기화
 */
void ShopPopup::initNavigator() {
    
    navigator = PackageNavigator::create();
    navigator->setOnPackageChangedListener(CC_CALLBACK_1(ShopPopup::onPackageChanged, this));
    addContentChild(navigator);
}

/**
 * 캐릭터 영역 초기화
 */
void ShopPopup::initCharacter() {
    
    characterView = CharacterView::create();
    addContentChild(characterView);
}

/**
 * 하단 메뉴 초기화
 */
void ShopPopup::initBottomMenu() {
    
    bottomMenu = SBNodeUtils::createZeroSizeNode();
    bottomMenu->setPosition(Vec2(0, -180));
    addChild(bottomMenu);
    
    // RSP_btn_left.png Vec2BC(-268, 100) , Size(160, 152)
    // RSP_btn_right.png Vec2BC(268, 100) , Size(160, 152)
    // RSP_btn_shop_select.png Vec2BC(0, 100) , Size(352, 152)
    // RSP_btn_shop_buy.png
    // RSP_btn_shop_play_movie.png
    vector<SBUIInfo> infos({
        SBUIInfo(Tag::BTN_LEFT,            ANCHOR_M,    Vec2BC(-268, 100),    "RSP_btn_left.png"),
        SBUIInfo(Tag::BTN_RIGHT,           ANCHOR_M,    Vec2BC(268, 100),     "RSP_btn_right.png"),
        SBUIInfo(Tag::BTN_SELECT,          ANCHOR_M,    Vec2BC(0, 100),       "RSP_btn_shop_select.png"),
        SBUIInfo(Tag::BTN_BUY,             ANCHOR_M,    Vec2BC(0, 100),       "RSP_btn_shop_buy.png"),
        SBUIInfo(Tag::BTN_VIEW_ADS,        ANCHOR_M,    Vec2BC(0, 100),       "RSP_btn_shop_play_movie.png"),
    });
    
    for( int i = 0; i < infos.size(); ++i ) {
        auto info = infos[i];
        string file = DIR_IMG_GAME + info.file;
        
        auto btn = SBButton::create(file);
        btn->setZoomScale(0.05f);
        info.apply(btn);
        bottomMenu->addChild(btn);
        
        btn->setOnClickListener(CC_CALLBACK_1(ShopPopup::onClick, this));
    }
    
    bottomMenu->getChildByTag(Tag::BTN_BUY)->setVisible(false);
    bottomMenu->getChildByTag(Tag::BTN_VIEW_ADS)->setVisible(false);
}

void ShopPopup::dismiss() {
    
    BasePopup::dismiss();
    
    // SceneManager::getCommonMenu()->getBottomMenu()->openMenu(0);
}

/**
 * 등장 연출
 */
void ShopPopup::runEnterAction(float duration, SBCallback onFinished) {
    
    duration *= enterTimeScale;
    BasePopup::runEnterAction(duration, onFinished);
    
    CCLOG("ShopPopup::runEnterAction");
    
    // 액션 완료
    auto onActionFinished = [=]() {
        CCLOG("ShopPopup::runEnterAction onActionFinished");
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    runSlideAction(onActionFinished, duration, SLIDE_OUT_POSITION, SLIDE_IN_POSITION);
    
    // 닫기 버튼으로 전환
    SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::BACK, duration);
    
    // 하단 메뉴 전환 액션
    const float bottomActionDuration = duration * 0.45f;
    SceneManager::getCommonMenu()->getBottomMenu()->closeMenu(bottomActionDuration);
    
    SBDirector::postDelayed(this, [=]() {
        SBActionHelper::runMoveAction(bottomMenu, Vec2(0, -180), Vec2(0, 0), bottomActionDuration);
    }, bottomActionDuration);
}

void ShopPopup::runEnterAction(SBCallback onFinished) {
    
    runEnterAction(SLIDE_IN_DURATION, onFinished);
}

/**
 * 퇴장 연출
 */
void ShopPopup::runExitAction(float duration, SBCallback onFinished) {
    
    duration *= exitTimeScale;
    BasePopup::runExitAction(duration, onFinished);
    
    CCLOG("ShopPopup::runExitAction");
    
    // slide out
    runSlideAction([=]() {
        CCLOG("ShopPopup::runExitAction onActionFinished");
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, duration, SLIDE_IN_POSITION, SLIDE_OUT_POSITION);
    
    // 설정 버튼으로 전환
    SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, duration);
    
    // 하단 메뉴 전환 액션
    const float bottomActionDuration = duration * 0.45f;
    SBActionHelper::runMoveAction(bottomMenu, Vec2(0, 0), Vec2(0, -180), bottomActionDuration);
    
    SBDirector::postDelayed(SceneManager::getCommonMenu(), [=]() {
        SceneManager::getCommonMenu()->getBottomMenu()->openMenu(bottomActionDuration);
    }, bottomActionDuration);
}

void ShopPopup::runExitAction(SBCallback onFinished) {
    
    runExitAction(SLIDE_OUT_DURATION, onFinished);
}

/**
 * 등장 연출 완료
 */
void ShopPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
