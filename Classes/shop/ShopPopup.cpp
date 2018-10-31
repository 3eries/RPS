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
#include "CommonLoadingBar.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

const float ShopPopup::SLIDE_IN_DURATION               = EffectDuration::POPUP_SLIDE_NORMAL;
const float ShopPopup::SLIDE_OUT_DURATION              = EffectDuration::POPUP_SLIDE_NORMAL;

#define                SLIDE_IN_POSITION               Vec2(0, 0)
#define                SLIDE_OUT_POSITION              Vec2TL(0, -SBNodeUtils::getBoundingBoxInWorld(characterView->getBackground()).getMinY())

static const string    SCHEDULER_CHECK_AD_LOADED       = "SCHEDULER_CHECK_AD_LOADED";

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
 
    charMgr->removeListener(this);
    AdsHelper::getInstance()->getEventDispatcher()->removeListener(this);
}

bool ShopPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    schedule([=](float dt) {
        this->updateViewAdsButton();
    }, 1.0f, SCHEDULER_CHECK_AD_LOADED);
    
    return true;
}

void ShopPopup::onEnter() {
    
    BasePopup::onEnter();
    
    // 캐릭터 리스너 초기화
    auto listener = CharacterListener::create();
    listener->setTarget(this);
    listener->onCharacterUnlocked = [=](Characters characters) {
        
        characterView->updateSelf();
        this->updateBottomMenu();
        
        PopupManager::showGetCharacterPopup(characters);
    };
    
    listener->onPackageUnlocked = [=](Packages packages) {
    };
    
    charMgr->addListener(listener);
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
    
    // 클릭 효과음
    if( tag != Tag::BTN_VIEW_ADS ) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    }
    
    switch( tag ) {
        // 이전/다음 캐릭터로 이동
        case Tag::BTN_LEFT:                 setCharacter(characterIndex-1);        break;
        case Tag::BTN_RIGHT:                setCharacter(characterIndex+1);        break;
            
        // 캐릭터 선택
        case Tag::BTN_SELECT: {
            charMgr->setSelected(getCharacter().charId);
            dismissWithAction();
            
        } break;
            
        // 캐릭터 구매
        case Tag::BTN_BUY: {
            onCharacterPurchase(getCharacter());
        } break;
            
        // 광고 시청
        case Tag::BTN_VIEW_ADS: {
            showAd();
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
 * 패키지 구매
 */
void ShopPopup::onPackagePurchase(Package pack) {
    
    CCLOG("ShopPopup::onPackagePurchase:\n%s", pack.toString().c_str());
    
    // MessageBox(STR_FORMAT("TODO: %s", pack.name.c_str()).c_str(), "");

    // 패키지 해제
    charMgr->unlockPackage(pack.packId);
}

/**
 * 캐릭터 구매
 */
void ShopPopup::onCharacterPurchase(Character character) {
    
    CCLOG("ShopPopup::onCharacterPurchase:\n%s", character.toString().c_str());
    
    // MessageBox(STR_FORMAT("TODO: %s", character.name.c_str()).c_str(), "");
    
    // 캐릭터 해제
    charMgr->unlockCharacter(character.charId);
}

/**
 * 광고 시청
 */
void ShopPopup::showAd() {
    
    if( !AdsHelper::isRewardedVideoLoaded() ) {
        return;
    }
    
    const auto character = getCharacter();
    
    auto loadingBar = CommonLoadingBar::create();
    loadingBar->setUIDelay(0.1f);
    loadingBar->show();
    
    auto listener = RewardedVideoAdListener::create();
    listener->setTarget(this);
    
    // 로딩 실패
    listener->onAdFailedToLoad = [=](int error) {
        firebase::Analytics::logEvent("debug_continue_rewarded_ad");
    };
    // 보상 완료
    listener->onRewarded = [=](string type, int amount) {
    };
    // 광고 open
    listener->onAdOpened = [=]() {
        loadingBar->dismissWithDelay(0);
    };
    // 광고 close
    listener->onAdClosed = [=]() {
        if( listener->isRewarded() ) {
            // submit & commit
            charMgr->submit(PackageDB::Field::VIEW_ADS, 1, character.charId);
            charMgr->commit(character.packId);
            
            // update ui
            characterView->updateSelf();
        }
        
        this->updateViewAdsButton();
    };
    
    AdsHelper::getInstance()->showRewardedVideo(listener);
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
    characterView->setCharacter(getCharacter());
    
    // 하단 메뉴 업데이트
    updateBottomMenu();
}

/**
 * 하단 메뉴 업데이트
 */
void ShopPopup::updateBottomMenu() {
    
    auto character = getCharacter();
    
    auto setVisible = [=](Tag tag, bool isVisible) {
        bottomMenu->getChildByTag(tag)->setVisible(isVisible);
    };
    
    Tag tags[] = {
        Tag::BTN_SELECT,
        Tag::BTN_BUY,
        Tag::BTN_VIEW_ADS,
        Tag::IMG_VIEW_ADS_DISABLED,
    };
    
    for( auto tag : tags ) {
        setVisible(tag, false);
    }
    
    // 잠금 해제됨
    if( charMgr->isCharacterUnlocked(character.charId) ) {
        setVisible(Tag::BTN_SELECT, true);
    }
    // 구매 가능
    else if( iap::IAPHelper::hasItem(character.charId) ) {
        setVisible(Tag::BTN_BUY, true);
    }
    // VIEW ADS 타입
    else if( character.unlockType == UnlockType::VIEW_ADS ) {
        // setVisible(Tag::BTN_VIEW_ADS, true);
        updateViewAdsButton();
    }
    else {
        CCLOG("ShopPopup::updateBottomMenu error: invalid character(id: %s).", character.charId.c_str());
    }
}

/**
 * 광고 시청 버튼 업데이트
 */
void ShopPopup::updateViewAdsButton() {
    
    auto character = getCharacter();
    
    if( character.charId == "" || character.unlockType != UnlockType::VIEW_ADS ) {
        return;
    }
    
    if( charMgr->isCharacterUnlocked(character.charId) ) {
        return;
    }
    
    const bool isAdLoaded = AdsHelper::isRewardedVideoLoaded();
    
    bottomMenu->getChildByTag(Tag::BTN_VIEW_ADS)->setVisible(isAdLoaded);
    bottomMenu->getChildByTag(Tag::IMG_VIEW_ADS_DISABLED)->setVisible(!isAdLoaded);
    
    if( isAdLoaded ) {
        // TODO: 스파인 연출 시작?
    }
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
    navigator->setOnPackagePurchaseListener(CC_CALLBACK_1(ShopPopup::onPackagePurchase, this));
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
    
    // VIEW ADS 비활성화 이미지
    {
        auto img = Sprite::create(DIR_IMG_GAME + "RSP_btn_shop_play_movie_dis.png");
        img->setTag(Tag::IMG_VIEW_ADS_DISABLED);
        img->setVisible(false);
        img->setAnchorPoint(ANCHOR_M);
        img->setPosition(Vec2BC(0, 100));
        bottomMenu->addChild(img);
    }
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
