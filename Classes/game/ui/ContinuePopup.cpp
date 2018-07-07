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
static const int    COUNTDOWN           = 10;

static const float ENTER_DURATION       = 0.2f;
static const float EXIT_DURATION        = 0.2f;

#define COUNTDOWN_POS_LEFT              Vec2MC(-800, 100)
#define COUNTDOWN_POS_CENTER            Vec2MC(0, 100)
#define COUNTDOWN_POS_RIGHT             Vec2MC(800, 100)

ContinuePopup::ContinuePopup() : BasePopup(Type::CONTINUE),
onVideoListener(nullptr),
onTimeOutListener(nullptr),
elapsed(0),
count(COUNTDOWN) {
    
}

ContinuePopup::~ContinuePopup() {
    
}

bool ContinuePopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    runEnterAction();
    
    return true;
}

void ContinuePopup::onExit() {
    
    getScheduler()->setTimeScale(1);
    
    BasePopup::onExit();
}

/**
 * 카운트 다운 시작
 */
void ContinuePopup::countdown() {
    
    auto updateCount = [=]() {
        int i = COUNTDOWN - elapsed;
        
        if( i < 0 ) {
            this->timeOut();
        } else {
            i = MAX(0, i);
            i = MIN(COUNTDOWN-1, i);
            
            count = i;
            this->updateCountdownImage();
//            countdownLabel->setString(TO_STRING(i));
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
    dismiss();
}

void ContinuePopup::updateCountdownImage() {
    
    string file = DIR_IMG_GAME + STR_FORMAT("RSP_continue_%02d.png", count);
    countdownImage->setTexture(file);
}

void ContinuePopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    // setBackgroundColor(Color::POPUP_BG);
    setBackgroundColor(Color4B(0,0,0,255*0.5f));
}

void ContinuePopup::initContentView() {
    
    BasePopup::initContentView();
    
    // 타이틀
    // text_continue.png Vec2TC(2, -222) , Size(580, 84)
    auto title = Sprite::create(DIR_IMG_GAME + "text_continue.png");
    title->setTag(Tag::IMG_TITLE);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(0, -222));
    addChild(title);
    
    // 카운트 다운
    // RSP_continue_10.png Vec2MC(0, 100) , Size(568, 416)
    countdownImage = Sprite::create(DIR_IMG_GAME + "RSP_continue_10.png");
    countdownImage->setAnchorPoint(ANCHOR_M);
    countdownImage->setPosition(COUNTDOWN_POS_CENTER);
    addChild(countdownImage);
    
    /*
    countdownLabel = Label::createWithTTF("", FONT_RETRO, 300);
    countdownLabel->setAnchorPoint(ANCHOR_M);
    countdownLabel->setPosition(Vec2MC(0, 100));
    countdownLabel->setColor(Color3B::WHITE);
    countdownLabel->enableShadow(Color4B(138,125,164,255), Size(4,-4), 10);
    addChild(countdownLabel);
    */
    
    // 비디오 버튼
    // btn_continue.png Vec2BC(0, 392) , Size(520, 152)
    auto videoBtn = SBButton::create(DIR_IMG_GAME + "btn_continue.png");
    videoBtn->setTag(Tag::BTN_VIDEO);
    videoBtn->setZoomScale(0.05f);
    videoBtn->setAnchorPoint(ANCHOR_M);
    videoBtn->setPosition(Vec2BC(0, 392));
    addChild(videoBtn, 1);
    
    videoBtn->setOnClickListener([=](Node*) {
        
        onVideoListener();
        this->dismiss();
    });
    
}

/**
 * 등장 연출
 */
void ContinuePopup::runEnterAction(SBCallback onFinished) {

    BasePopup::runEnterAction(onFinished);
    
    // 배경
    runBackgroundFadeInAction(nullptr, 0.15f);
    
    // 타이틀
    auto scale1 = ScaleTo::create(0.15f, 1.1f);
    auto scale2 = ScaleTo::create(0.05f, 1.0f);
    auto scaleSeq = Sequence::create(scale1, scale2, nullptr);
    getChildByTag(Tag::IMG_TITLE)->runAction(scaleSeq->clone());
    
    // 버튼
    getChildByTag(Tag::BTN_VIDEO)->runAction(scaleSeq->clone());
    
    // 콜백
    auto delay = DelayTime::create(ENTER_DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 퇴장 연출
 */
void ContinuePopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(onFinished);
    
    // 배경
    runBackgroundFadeOutAction(nullptr, 0.15f);
    
    // 타이틀
    auto scale = ScaleTo::create(0.15f, 0);
    getChildByTag(Tag::IMG_TITLE)->runAction(scale->clone());
    
    // 버튼
    getChildByTag(Tag::BTN_VIDEO)->runAction(scale->clone());
    
    // 콜백
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    auto delay = DelayTime::create(EXIT_DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        SBDirector::getInstance()->setScreenTouchLocked(false);
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 등장 연출 완료
 */
void ContinuePopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    countdown();
}
