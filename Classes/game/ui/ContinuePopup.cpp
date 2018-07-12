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
using namespace spine;
using namespace std;

static const string SCHEDULER_COUNTDOWN = "SCHEDULER_COUNTDOWN";
static const int    COUNTDOWN_START     = 10;

static const float ENTER_DURATION       = 0.2f;
static const float EXIT_DURATION        = 0.2f;

#define COUNTDOWN_POS_LEFT              Vec2MC(-800, 100)
#define COUNTDOWN_POS_CENTER            Vec2MC(0, 100)
#define COUNTDOWN_POS_RIGHT             Vec2MC(800, 100)

ContinuePopup::ContinuePopup() : BasePopup(Type::CONTINUE),
onVideoListener(nullptr),
onTimeOutListener(nullptr),
count(COUNTDOWN_START) {
    
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
 * 카운트 다운
 */
void ContinuePopup::countdown() {
    
    countdownAnim->clearTracks();
    countdownAnim->setAnimation(0, TO_STRING(count), false);
}

/**
 * 타임 아웃
 */
void ContinuePopup::timeOut() {
    
    onTimeOutListener();
    
    unscheduleAllCallbacks();
    dismiss();
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
    /*
    auto title = Sprite::create(DIR_IMG_GAME + "text_continue.png");
    title->setTag(Tag::IMG_TITLE);
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(0, -222));
    addChild(title);
     */
    
    // 카운트 다운
    countdownAnim = SkeletonAnimation::createWithJsonFile(ANIM_CONTINUE);
    countdownAnim->setAnchorPoint(ANCHOR_M);
    countdownAnim->setPosition(Vec2MC(0, 0));
    countdownAnim->setVisible(false);
    addChild(countdownAnim);
    
    // 애니메이션 시작 리스너
    countdownAnim->setStartListener([=](spTrackEntry *entry) {
        
        CCLOG("countdown animation start: %s", entry->animation->name);
        // countdownAnim->setVisible(true);
        countdownAnim->setTimeScale(1);
    });
    
    // 애니메이션 완료 리스너
    countdownAnim->setCompleteListener([=](spTrackEntry *entry) {

        CCLOG("countdown animation completed: %s", entry->animation->name);
        
        if( count > 0 ) {
            count--;
            this->countdown();
        } else {
            this->timeOut();
        }
    });
    
    /*
    countdownLabel = Label::createWithTTF("", FONT_RETRO, 300);
    countdownLabel->setAnchorPoint(ANCHOR_M);
    countdownLabel->setPosition(Vec2MC(0, 100));
    countdownLabel->setColor(Color3B::WHITE);
    countdownLabel->enableShadow(Color4B(138,125,164,255), Size(4,-4), 10);
    addChild(countdownLabel);
    */
    
    // 터치 영역
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
        
        if( eventType == Widget::TouchEventType::BEGAN ) {
            countdownAnim->setTimeScale(5.0f);
        }
    });
    
    // 비디오 버튼
    // btn_continue.png Vec2BC(0, 392) , Size(520, 152)
    /*
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
    */
    auto videoBtn = Widget::create();
    videoBtn->setAnchorPoint(ANCHOR_M);
    videoBtn->setPosition(Vec2BC(0, 392));
    videoBtn->setContentSize(Size(520, 152));
    videoBtn->setTouchEnabled(true);
    addChild(videoBtn, 1);
    
    // videoBtn->addChild(SBNodeUtils::createBackgroundNode(videoBtn, Color4B(255,0,0,255*0.5f)));
    
    videoBtn->addClickEventListener([=](Ref*) {
        onVideoListener();
        this->dismiss();
    });
}

/**
 * 등장 연출
 */
void ContinuePopup::runEnterAction(SBCallback onFinished) {

    BasePopup::runEnterAction(onFinished);
    
    // 터치 잠금
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    // 카운트 다운
    countdownAnim->setVisible(true);
    countdown();
    
    // 배경
    runBackgroundFadeInAction(nullptr, 0.15f);
    
    // 타이틀
    /*
    auto scale1 = ScaleTo::create(0.15f, 1.1f);
    auto scale2 = ScaleTo::create(0.05f, 1.0f);
    auto scaleSeq = Sequence::create(scale1, scale2, nullptr);
    getChildByTag(Tag::IMG_TITLE)->runAction(scaleSeq->clone());
    
    // 버튼
    getChildByTag(Tag::BTN_VIDEO)->runAction(scaleSeq->clone());
     */
    
    // 콜백
    auto delay = DelayTime::create(ENTER_DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
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
    
    /*
    // 타이틀
    auto scale = ScaleTo::create(0.15f, 0);
    getChildByTag(Tag::IMG_TITLE)->runAction(scale->clone());
    
    // 버튼
    getChildByTag(Tag::BTN_VIDEO)->runAction(scale->clone());
     */
    
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
}
