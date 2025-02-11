//
//  PackageNavigator.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#include "PackageNavigator.hpp"

#include <spine/spine-cocos2dx.h>

#include "Define.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

#define               FIRST_INDEX                   1
static const Size     NAVIGATION_ITEM_SIZE          = Size(560, 152);
#define               NAVIGATION_ITEM_MARGIN        (-(SB_WIN_SIZE.width - NAVIGATION_ITEM_SIZE.width - 20))
static const float    NAVIGATION_TITLE_TEXT_WIDTH   = 430;

PackageNavigator* PackageNavigator::create() {
    
    auto navigator = new PackageNavigator();
    
    if( navigator && navigator->init() ) {
        navigator->autorelease();
        return navigator;
    }
    
    delete navigator;
    return nullptr;
}

PackageNavigator::PackageNavigator() :
onPackageChangedListener(nullptr),
onPackagePurchaseListener(nullptr),
pageDiffCount(0) {
}

PackageNavigator::~PackageNavigator() {
    
}

bool PackageNavigator::init() {
    
    if( !Widget::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    // 데이터 초기화
    auto charMgr = CharacterManager::getInstance();
    auto packOrder = charMgr->getPackageOrder();
    
    for( string packId : packOrder ) {
        packs.push_back(charMgr->getPackage(packId));
    }
    
    // UI 초기화
    initItems();
    
    return true;
}

void PackageNavigator::onEnter() {
    
    Widget::onEnter();
    
    pageView->setCurrentPageIndex(FIRST_INDEX);
    onPackageChanged(FIRST_INDEX);
    
    showArrows();
}

/**
 * 패키지 전환
 */
void PackageNavigator::onPackageChanged(size_t i) {
   
    CCLOG("onPackageChanged: %d", i);
    
    pageIndex = i;
    onPackageChangedListener(packs[i]);
}

/**
 * 패키지 구매 고고
 */
void PackageNavigator::onPackagePurchase(size_t i) {
    
    CCLOG("onPackagePurchase: %d", i);
    
    if( pageIndex == i ) {
        onPackagePurchaseListener(packs[i]);
    }
}

/**
 * 스크롤 이벤트
 */
void PackageNavigator::onScrollEvent(ScrollView::EventType type) {
    
    switch( type ) {
        // 페이지 스크롤 진행중
        case ScrollView::EventType::CONTAINER_MOVED: {
            // CCLOG("CONTAINER_MOVED: %zd", pageView->getCurrentPageIndex());
            
            ssize_t i = pageView->getCurrentPageIndex();
            
            if( i < 0 ) {
                return;
            }
            
            if( pageIndex == i ) {
                return;
            }
            
            ++pageDiffCount;

            // 페이지 전환
            if( pageDiffCount == 3 ) {
                pageDiffCount = 0;
                onPackageChanged(i);
            }
            
//            if( i >= 0 && pageIndex != i ) {
//                onPackageChanged(i);
//            }
            
        } break;
            
        // 스크롤 시작
        case ScrollView::EventType::SCROLLING_BEGAN: {
            CCLOG("SCROLLING_BEGAN");
            
            auto items = pageView->getItems();
            
            for( auto item : items ) {
                auto naviItem = (NavigationItem*)item;
                naviItem->hideArrows();
            }
            
        } break;
            
        // 스크롤 완료
        case ScrollView::EventType::AUTOSCROLL_ENDED: {
            CCLOG("AUTOSCROLL_ENDED prev: %zd curr: %zd",
                  pageView->getPreviousPageIndex(),
                  pageView->getCurrentPageIndex());
            
            pageDiffCount = 0;
            
            if( pageIndex != pageView->getCurrentPageIndex() ) {
                onPackageChanged(pageView->getCurrentPageIndex());
            }
            
            showArrows();
            
        } break;
            
        case ScrollView::EventType::SCROLLING_ENDED: {
            CCLOG("SCROLLING_ENDED prev: %zd curr: %zd",
                  pageView->getPreviousPageIndex(),
                  pageView->getCurrentPageIndex());

            // showArrows();
            
//            auto item = getCurrentNavigationItem();
//            item->showArrow(NavigationItem::ArrowType::LEFT);
//            item->showArrow(NavigationItem::ArrowType::RIGHT);
            
        } break;
            
        case ScrollView::EventType::SCROLLING: {
            // CCLOG("SCROLLING: %zd", pageView->getCurrentPageIndex());
            
        } break;
            
        default:
            break;
    }
}

/**
 * 화살표 초기화
 */
void PackageNavigator::showArrows() {
    
    // size_t i = pageView->getCurrentPageIndex();
    size_t i = pageIndex;
    auto item = (NavigationItem*)pageView->getItem(i);
    
    if( i > 0 )                               item->showArrow(NavigationItem::ArrowType::LEFT);
    if( i < pageView->getItems().size()-1 )   item->showArrow(NavigationItem::ArrowType::RIGHT);
}

/**
 * 네비게이션 아이템 초기화
 */
void PackageNavigator::initItems() {
    
    // PageView 초기화
    pageView = PageView::create();
    pageView->setDirection(PageView::Direction::HORIZONTAL);
    pageView->setAnchorPoint(ANCHOR_M);
    pageView->setPosition(Vec2TC(0, -316));
    pageView->setContentSize(Size(SB_WIN_SIZE.width, 152));
    pageView->setItemsMargin(NAVIGATION_ITEM_MARGIN);
    pageView->setClippingEnabled(false);
    pageView->setBounceEnabled(true);
    // pageView->setAutoScrollStopEpsilon(0.1f);
    // pageView->setPadding(80, 0, 80, 0);
    addChild(pageView);
    
    /*
    {
        auto n = LayerColor::create(Color4B(255,0,0,255*0.4f));
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(pageView->getAnchorPoint());
        n->setPosition(pageView->getPosition());
        n->setContentSize(pageView->getContentSize());
        addChild(n);
    }
    */
    
    // 스크롤 이벤트 리스너
    pageView->addEventListener([=](Ref*, ScrollView::EventType type) {
        this->onScrollEvent(type);
    });
    
//    pageView->addEventListener([=](Ref *sender, PageView::EventType eventType) {
//        if( eventType == PageView::EventType::TURNING ) {
//            this->onTurning(pageView->getCurrentPageIndex());
//        }
//    });

    // 아이템 초기화
    for( int i = 0; i < packs.size(); ++i ) {
        auto pack = packs[i];
        
        auto item = NavigationItem::create(pack);
        item->hideArrows(false);
        pageView->addPage(item);
        
        // 클릭 리스너
        item->setOnPurchaseListener([=](Node*) {
            this->onPackagePurchase(i);
        });
    }
}

#pragma mark- NavigationItem

NavigationItem* NavigationItem::create(Package pack) {
    
    auto item = new NavigationItem(pack);
    
    if( item && item->init() ) {
        item->autorelease();
        return item;
    }
    
    delete item;
    return nullptr;
}

NavigationItem::NavigationItem(Package pack) :
pack(pack),
onPurchaseListener(nullptr),
background(nullptr),
unlockedMark(nullptr),
buyMark(nullptr) {
}

NavigationItem::~NavigationItem() {
    
    CharacterManager::getInstance()->removeListener(this);
}

bool NavigationItem::init() {
    
    if( !Widget::init() ) {
        return false;
    }
    
    const bool validPack = (pack.packId != "");
    
    // 배경
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_shop_pack_title.png");
    addChild(bg);
    
    background = bg;
    
    setContentSize(Size(SB_WIN_SIZE.width, bg->getContentSize().height));
    
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(getContentSize(), 0, 0));
    
    // 타이틀
    string title = validPack ? pack.name : "COMING SOON";
    
    auto titleLabel = Label::createWithTTF(title, FONT_SABO, 52, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(getContentSize(), 0, 0 + 5));
    titleLabel->setTextColor(Color4B(70,70,70,255));
    addChild(titleLabel);
    
    // 타이틀 크기 조절
    if( titleLabel->getContentSize().width > NAVIGATION_TITLE_TEXT_WIDTH ) {
        titleLabel->setScale(NAVIGATION_TITLE_TEXT_WIDTH / titleLabel->getContentSize().width);
    }
    
    // 타이틀 영역 확인용
    /*
    {
        auto n = LayerColor::create(Color4B(255,0,0,255*0.2f));
        n->setIgnoreAnchorPointForPosition(false);
        n->setAnchorPoint(ANCHOR_M);
        n->setPosition(Vec2MC(getContentSize(), 0, 0 + 5));
        n->setContentSize(Size(NAVIGATION_TITLE_TEXT_WIDTH, 20));
        addChild(n);
    }
    */
     
    // 화살표
    auto arrowLeft = Sprite::create(DIR_IMG_GAME + "icon_shop_pack_title_navi.png");
    arrowLeft->setAnchorPoint(ANCHOR_M);
    arrowLeft->setPosition(Vec2MC(bg->getContentSize(), -242, 6));
    arrowLeft->setFlippedX(true);
    bg->addChild(arrowLeft);
    
    arrows[ArrowType::LEFT] = arrowLeft;
    
    auto arrowRight = Sprite::create(DIR_IMG_GAME + "icon_shop_pack_title_navi.png");
    arrowRight->setAnchorPoint(ANCHOR_M);
    arrowRight->setPosition(Vec2MC(bg->getContentSize(), 242, 6));
    bg->addChild(arrowRight);
    
    arrows[ArrowType::RIGHT] = arrowRight;
    
    if( !validPack ) {
        return true;
    }
    
    // 패키지 잠금 해제 마크
    unlockedMark = Sprite::create(DIR_IMG_GAME + "RSP_sub_title_purple.png");
    unlockedMark->setVisible(false);
    unlockedMark->setAnchorPoint(ANCHOR_M);
    unlockedMark->setPosition(Vec2MC(bg->getContentSize(), 0, 88));
    bg->addChild(unlockedMark, -1);
    
    // 패키지 잠금 해제됨
    if( CharacterManager::getInstance()->isPackageUnlocked(pack.packId) ) {
        unlockedMark->setVisible(true);
    }
    // 구매 가능 패키지
    else if( iap::IAPHelper::hasItem(pack.packId) ) {
        setTouchEnabled(true);
        
        auto onClickBuy = [=]() {
            SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
            onPurchaseListener(this);
        };
        
        addClickEventListener([=](Ref*) {
            onClickBuy();
        });
        
        // 구매 가능 마크
        // RSP_sub_title_red.png Vec2MC(0, 88) , Size(360, 64)
        auto buyBtn = Button::create(DIR_IMG_GAME + "RSP_sub_title_red.png");
        buyBtn->setZoomScale(0);
        buyBtn->setAnchorPoint(ANCHOR_M);
        buyBtn->setPosition(Vec2MC(bg->getContentSize(), 0, 88));
        bg->addChild(buyBtn, -1);
        
        buyMark = buyBtn;
        
        buyBtn->addClickEventListener([=](Ref*) {
            onClickBuy();
        });
        
        auto scale1 = ScaleTo::create(0.5f, 1.05f);
        auto scale2 = ScaleTo::create(0.5f, 1.0f);
        auto seq = Sequence::create(scale1, scale2, nullptr);
        buyBtn->runAction(RepeatForever::create(seq));
    }
    // 구매할 수 없는 패키지
    else {
        
    }
    
    return true;
}

void NavigationItem::onEnter() {
    
    Widget::onEnter();
    
    // 캐릭터 리스너 초기화
    auto listener = CharacterListener::create();
    listener->setTarget(this);
    
    // 패키지 해제
    auto onPackageUnlocked = [=](Packages packages) {
        
        bool found = false;
        
        for( auto pack : packages ) {
            if( this->pack.packId == pack.packId ) {
                found = true;
                break;
            }
        }
        
        if( found ) {
            this->setTouchEnabled(false);
            SB_SAFE_SHOW(unlockedMark);
            SB_SAFE_HIDE(buyMark);
        }
    };
    
    listener->onPackageUnlocked = onPackageUnlocked;
    listener->onPackageRestored = onPackageUnlocked;
    
    CharacterManager::getInstance()->addListener(listener);
}

void NavigationItem::onExit() {
    
    CharacterManager::getInstance()->removeListener(this);
    
    Widget::onExit();
}

void NavigationItem::showArrow(ArrowType type) {
    
    auto arrow = arrows[type];
    arrow->stopAllActions();
    arrow->runAction(FadeIn::create(0.05f));
}

void NavigationItem::hideArrow(ArrowType type) {
    
    auto arrow = arrows[type];
    arrow->stopAllActions();
    arrow->runAction(FadeOut::create(0.05f));
}

void NavigationItem::hideArrows(bool isAction) {
    
    if( isAction ) {
        hideArrow(ArrowType::LEFT);
        hideArrow(ArrowType::RIGHT);
    } else {
        arrows[ArrowType::LEFT]->setOpacity(0);
        arrows[ArrowType::RIGHT]->setOpacity(0);
    }
}
