//
//  RankingPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 17..
//

#include "RankingPopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"
#include "SceneManager.h"

#include "PopupManager.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const float RankingPopup::SLIDE_IN_DURATION               = 0.2f;
const float RankingPopup::SLIDE_OUT_DURATION              = 0.2f;
const float RankingPopup::FADE_IN_DURATION                = 0.5f;
const float RankingPopup::FADE_OUT_DURATION               = 0.5f;

const float RankingPopup::CLOSE_BUTTON_SLIDE_DURATION     = 0.2f;

#define CLOSE_BUTTON_POS_X                       Vec2TC(303, -51)
#define CLOSE_BUTTON_POS_X_BANNER                Vec2TC(303, -153)

RankingPopup::RankingPopup(Type type) : BasePopup(type) {
    
}

RankingPopup::~RankingPopup() {
    
}

bool RankingPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    initRankings();
    
    return true;
}

void RankingPopup::onEnter() {
    
    BasePopup::onEnter();
}

void RankingPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
}

void RankingPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // stone bg
    // RSP_popup_bg_big.png Vec2MC(0, 46) , Size(696, 956)
    stone = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_big.png");
    stone->setAnchorPoint(ANCHOR_M);
    stone->setPosition(Vec2MC(0, 46));
    contentView->addChild(stone);
    
    // title
    {
        // RSP_popup_title_ranking.png Vec2MC(0, 332) , Size(320, 56)
        SBUIInfo infos[] = {
            SBUIInfo(ANCHOR_M,   Vec2MC(0, 332),  "RSP_popup_title_ranking.png"),      // ranking
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto spr = Sprite::create(DIR_IMG_GAME + info.file);
            info.apply(spr);
            contentView->addChild(spr);
            
            fadeNodes.push_back(spr);
        }
    }
    
    // 닫기 버튼
    // RSP_btn_go_back.png Vec2TC(303, -153) , Size(100, 90) // 배너 좌표 고려?
    // RSP_btn_go_back.png Vec2TC(303, -51) , Size(100, 90)
    // RSP_btn_option.png Vec2TC(298, -158) , Size(92, 92)   // 배너 좌표 고려?
    // RSP_btn_option.png Vec2TC(298, -56) , Size(92, 92)
    // SBUIInfo(Tag::SETTING,       ANCHOR_TR,   Vec2TR(-10, -10),  "RSP_btn_option.png"),
    // 720-10-92*0.5   = 664
    // 1280-10-92*0.5  = 1224
    closeBtn = SBButton::create(DIR_IMG_GAME + "RSP_btn_go_back.png");
    closeBtn->setAnchorPoint(ANCHOR_M);
    closeBtn->setPosition(SceneManager::isBannerVisible() ? CLOSE_BUTTON_POS_X_BANNER : CLOSE_BUTTON_POS_X);
    contentView->addChild(closeBtn);
    
    closeBtn->setOnClickListener([=](Node*) {
        this->dismissWithAction();
    });
    
    fadeNodes.push_back(closeBtn);
}

void RankingPopup::initRankings() {
    
    // clipping node
    auto stencil = LayerColor::create(Color4B::RED);
    stencil->setIgnoreAnchorPointForPosition(false);
    stencil->setAnchorPoint(ANCHOR_M);
    stencil->setPosition(Vec2MC(0, -25));
    stencil->setContentSize(Size(RecordRowView::SIZE.width + 60, 610));
    // rankingView->addChild(stencil);
    
    auto clippingNode = ClippingNode::create(stencil);
    clippingNode->setAnchorPoint(ANCHOR_M);
    clippingNode->setPosition(Vec2MC(0, 0));
    clippingNode->setContentSize(SB_WIN_SIZE);
    addContentChild(clippingNode);
    
    // 랭킹 리스트
    rankingView = SBNodeUtils::createZeroSizeNode();
    clippingNode->addChild(rankingView);
    
    fadeNodes.push_back(rankingView);
    
    const float paddingY = 60;
    float posY = 243;
    
    // 1.BSJ - 1500 Vec2MC(-1, 243) , Size(376, 34)
    // 2.BSJ - 1500 Vec2MC(-1, 183) , Size(376, 34)
    // 10.BSJ - 1500 Vec2MC(-17, -298) , Size(408, 34)
    auto records = getRecords();
    
    for( int i = 0; i < records.size(); ++i ) {
        auto record = records[i];

        auto rowView = RecordRowView::create(record);
        rowView->setPosition(Vec2MC(0, posY));
        rankingView->addChild(rowView);
        
        recordRowViews.push_back(rowView);
        
        posY -= paddingY;
    }
}

RankingRecords RankingPopup::getRecords() {
    return RankingManager::getTopRecords(10);
}

/**
 * 등장 연출
 */
void RankingPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(onFinished);
    
    // 터치 잠금
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    // 액션 완료
    auto onActionFinished = [=]() {
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    // 다른 팝업 있음, 연출 없음
    if( PopupManager::getInstance()->exists(BasePopup::Type::GAME_OVER) ) {
         onActionFinished();
    }
    // 다른 팝업 없음, slide in
    else {
        runSlideInAction(onActionFinished, SLIDE_IN_DURATION);
        
        /*
        runSlideInAction([=]() {
            
            SBDirector::getInstance()->setScreenTouchLocked(false);
            this->onEnterActionFinished();
            
            SB_SAFE_PERFORM_LISTENER(this, onFinished);
            
        }, SLIDE_IN_DURATION);
        */
        
        // 닫기 버튼 <--> 연출
        if( closeBtn->isVisible() ) {
    //        closeBtn->setPosition(SceneManager::isBannerVisible() ? CLOSE_BUTTON_POS_X_BANNER : CLOSE_BUTTON_POS_X);
            /*
            float originX = closeBtn->getPositionX();
            float posX = SB_WIN_SIZE.width + (closeBtn->getContentSize().width*0.5f);
            closeBtn->setPosition(Vec2(posX, closeBtn->getPositionY()));
            
            auto delay = DelayTime::create(0.2f);
            auto move = MoveTo::create(CLOSE_BUTTON_SLIDE_DURATION, Vec2(originX, closeBtn->getPositionY()));
            closeBtn->runAction(Sequence::create(delay, move, nullptr));
            */
        }
    }
}

/**
 * 퇴장 연출
 */
void RankingPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(onFinished);
    
    // 터치 잠금
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    // 액션 완료
    auto onActionFinished = [=]() {
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    // 다른 팝업 있음, fade out
    if( PopupManager::getInstance()->exists(BasePopup::Type::GAME_OVER) ) {
        // 연출 없음
        onActionFinished();
        
        // fade out
        /*
        stone->setVisible(false);
        
        for( auto n : fadeNodes ) {
            SBNodeUtils::recursiveCascadeOpacityEnabled(n, true);
            
            n->setOpacity(255);
            n->runAction(FadeOut::create(FADE_OUT_DURATION));
        }
        
        auto delay = DelayTime::create(FADE_OUT_DURATION * 1.05);
        auto callFunc = CallFunc::create(onActionFinished);
        runAction(Sequence::create(delay, callFunc, nullptr));
         */
    }
    // 다른 팝업 없음, slide out
    else {
        runSlideOutAction(onActionFinished, SLIDE_OUT_DURATION);
        
        /*
        runSlideOutAction([=]() {
            
            SBDirector::getInstance()->setScreenTouchLocked(false);
            this->onExitActionFinished();
            
            SB_SAFE_PERFORM_LISTENER(this, onFinished);
            
        }, SLIDE_OUT_DURATION);
         */
    }
}

void RankingPopup::setCloseButtonEnabled(bool enabled) {
    
    closeBtn->setVisible(enabled);
}

