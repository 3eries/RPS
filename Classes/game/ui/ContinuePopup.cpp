//
//  ContinuePopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ContinuePopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const string SCHEDULER_COUNTDOWN = "SCHEDULER_COUNTDOWN";
static const int    COUNTDOWN           = 11;

ContinuePopup::ContinuePopup() : SBBasePopup(),
onClosedListener(nullptr),
onVideoListener(nullptr),
onTimeOutListener(nullptr),
elapsed(0) {
    
}

ContinuePopup::~ContinuePopup() {
    
}

bool ContinuePopup::init() {
    
    if( !SBBasePopup::init() ) {
        return false;
    }
    
    initBg();
    initMenu();
    
    // 연출
    contentLayer->setPositionY(SB_WIN_SIZE.height);
    
    auto move = MoveTo::create(0.3f, Vec2(contentLayer->getPositionX(), 0));
    auto callFunc = CallFunc::create([=]() {
        this->countdown();
    });
    contentLayer->runAction(Sequence::create(move, callFunc, nullptr));
    
    return true;
}

void ContinuePopup::onExit() {
    
    SBBasePopup::onExit();
}

/**
 * 팝업 제거
 */
void ContinuePopup::close() {
    
    onClosedListener();
    removeFromParent();
}

/**
 * 카운트 다운 시작
 */
void ContinuePopup::countdown() {
    
    auto updateCount = [=]() {
        int i = COUNTDOWN - elapsed;
        i = MAX(0, i);
        
        if( i == 0 ) {
            this->timeOut();
        } else {
            countdownLabel->setString(TO_STRING(i-1));
        }
    };
    
    updateCount();
    
    // 터치 영역 생성
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        if( eventType == Widget::TouchEventType::BEGAN ) {
            // 터치 시 1초 감소
            elapsed += 1;
            updateCount();
        }
    });
    
    // 스케줄러 실행
    elapsed = 0;
    
    schedule([=](float dt) {
        elapsed += dt;
        updateCount();
    }, SCHEDULER_COUNTDOWN);
}

/**
 * 타임 아웃
 */
void ContinuePopup::timeOut() {
    
    onTimeOutListener();
    
    unscheduleAllCallbacks();
    close();
}

void ContinuePopup::initBg() {
    
    auto bg = LayerColor::create(Color::POPUP_BG);
    addChild(bg);
    
    // background fade in
    {
        bg->setOpacity(0);
        bg->runAction(FadeTo::create(0.2f, 255*0.3f));
    }
    
    contentLayer = SBNodeUtils::createZeroSizeNode();
    addChild(contentLayer);
    
    // 타이틀
    auto titleLabel = Label::createWithTTF("Continue?", FONT_RETRO, 95);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(0, 350));
    titleLabel->setColor(Color3B::WHITE);
//    titleLabel->enableOutline(Color4B::BLACK, 3);
    titleLabel->enableShadow(Color4B(138,125,164,255), Size(4,-4), 10);
    contentLayer->addChild(titleLabel);
    
    // 카운트 다운
    countdownLabel = Label::createWithTTF("", FONT_RETRO, 300);
    countdownLabel->setAnchorPoint(ANCHOR_M);
    countdownLabel->setPosition(Vec2MC(0, 100));
    countdownLabel->setColor(Color3B::WHITE);
    countdownLabel->enableShadow(Color4B(138,125,164,255), Size(4,-4), 10);
    contentLayer->addChild(countdownLabel);
}

void ContinuePopup::initMenu() {
    
    // 비디오 버튼
    auto videoBtn = SBButton::create(SBButton::Config("", Size(500, 80),
                                                      "VIDEO (INSERT COIN)", FONT_RETRO, 35));
    videoBtn->setZoomScale(0.1f);
    videoBtn->setAnchorPoint(ANCHOR_M);
    videoBtn->setPosition(Vec2BC(0, 370));
    addChild(videoBtn, SBZOrder::BOTTOM);
    
    {
        auto titleLabel = videoBtn->getTitle();
        titleLabel->setColor(Color3B::WHITE);
        titleLabel->enableOutline(Color4B::BLACK, 3);
    }
    
    videoBtn->setOnClickListener([=](Node*) {
        
        onVideoListener();
        this->close();
    });
}
