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

const float RankingPopup::SLIDE_IN_DURATION               = EffectDuration::POPUP_SLIDE_NORMAL;
const float RankingPopup::SLIDE_OUT_DURATION              = EffectDuration::POPUP_SLIDE_NORMAL;

#define                   SLIDE_IN_POSITION               Vec2(0, 0)
#define                   SLIDE_OUT_POSITION              Vec2TL(0, -stone->getBoundingBox().getMinY())

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
    
    // addContentChild(SBNodeUtils::createBackgroundNode(getContentView(), Color4B(0,0,255,255*0.3f)), -1);
    
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
        }
    }
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
void RankingPopup::runEnterAction(float duration, SBCallback onFinished) {
    
    duration *= enterTimeScale;
    BasePopup::runEnterAction(duration, onFinished);
    
    CCLOG("RankingPopup::runEnterAction");
    
    // 액션 완료
    auto onActionFinished = [=]() {
        CCLOG("RankingPopup::runEnterAction onActionFinished");
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    // slide
    runSlideAction(onActionFinished, duration, SLIDE_OUT_POSITION, SLIDE_IN_POSITION);
    
    // 닫기 버튼으로 전환
    if( type == Type::RANKING ) {
        SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::BACK, duration);
    }
}

void RankingPopup::runEnterAction(SBCallback onFinished) {
    
    runEnterAction(SLIDE_IN_DURATION, onFinished);
}

/**
 * 퇴장 연출
 */
void RankingPopup::runExitAction(float duration, SBCallback onFinished) {
    
    duration *= exitTimeScale;
    BasePopup::runExitAction(duration, onFinished);
    
    CCLOG("RankingPopup::runExitAction");
    
    // 액션 완료
    auto onActionFinished = [=]() {
        CCLOG("RankingPopup::runExitAction onActionFinished");
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    // slide
    runSlideAction(onActionFinished, duration, SLIDE_IN_POSITION, SLIDE_OUT_POSITION);
    
    // 설정 버튼으로 전환
    if( type == Type::RANKING ) {
        SceneManager::getCommonMenu()->getTopMenu()->setRightMenu(TopMenu::Tag::SETTING, duration);
    }
}

void RankingPopup::runExitAction(SBCallback onFinished) {
    
    runExitAction(SLIDE_OUT_DURATION, onFinished);
}

/**
 * 등장 연출 완료
 */
void RankingPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    SBAudioEngine::playBGM(SOUND_BGM_RECORD);
}

/**
 * 퇴장 연출 완료
 */
void RankingPopup::onExitActionFinished() {
    
    BasePopup::onExitActionFinished();
    
    if( getType() == BasePopup::Type::RANKING ) {
        SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }
}
