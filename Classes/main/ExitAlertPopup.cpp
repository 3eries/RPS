//
//  ExitAlertPopup.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#include "ExitAlertPopup.hpp"

#include "RSP.h"
#include "User.hpp"
#include "UIHelper.hpp"
#include "UserDefaultKey.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const float FADE_DURATION    = 0.15f;
static const float SLIDE_DURATION   = 0.2f;

ExitAlertPopup::ExitAlertPopup() : BasePopup(Type::EXIT_APP),
onExitAppListener(nullptr) {
    
}

ExitAlertPopup::~ExitAlertPopup() {
}

bool ExitAlertPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ExitAlertPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

void ExitAlertPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void ExitAlertPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_give_up.png Vec2MC(0, 0) , Size(552, 568)
    stoneBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_give_up.png");
    stoneBg->setAnchorPoint(ANCHOR_M);
    stoneBg->setPosition(Vec2MC(0, 0));
    addContentChild(stoneBg);
    
    const Size SIZE(stoneBg->getContentSize());
    
    // title
    // RSP_popup_title_give_up.png Vec2MC(0, 210) , Size(392, 60)
    auto title = Sprite::create(DIR_IMG_GAME + "RSP_popup_title_give_up.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2MC(SIZE, 0, 210));
    stoneBg->addChild(title);
    
    // god's hand
    // RSP_icon_scissors.png Vec2MC(-3, 41) , Size(104, 88)
    const auto myHand = getRandomHand();
    
    {
        string file = "";
        
        switch( myHand ) {
            case RSPType::ROCK:         file = "RSP_icon_rock.png";         break;
            case RSPType::SCISSORS:     file = "RSP_icon_scissors.png";     break;
            case RSPType::PAPER:        file = "RSP_icon_paper.png";        break;
            default:
                break;
        }
        
        auto spr = Sprite::create(DIR_IMG_GAME + file);
        spr->setAnchorPoint(ANCHOR_M);
        spr->setPosition(Vec2MC(SIZE, 0, 41));
        stoneBg->addChild(spr);
    }
    
    // my choice
    // RSP_btn_rock_quit.png Vec2MC(-128, -172) , Size(240, 152)
    // RSP_btn_paper_quit.png Vec2MC(128, -172) , Size(240, 152)
    {
        auto getFile = [](RSPType hand) -> string {
            switch( hand ) {
                case RSPType::ROCK:         return "RSP_btn_rock_quit.png";
                case RSPType::SCISSORS:     return "RSP_btn_scissors_quit.png";
                case RSPType::PAPER:        return "RSP_btn_paper_quit.png";
                default:
                    break;
            }
            
            return "";
        };
        
        vector<Vec2> POS = { Vec2MC(SIZE, -128, -172), Vec2MC(SIZE, 128, -172), };
        random_shuffle(POS.begin(), POS.end());
        
        // Win Hand
        auto winHandBtn = SBButton::create(DIR_IMG_GAME + getFile(getWinHand(myHand)));
        winHandBtn->setZoomScale(0);
        winHandBtn->setAnchorPoint(ANCHOR_M);
        winHandBtn->setPosition(POS[0]);
        stoneBg->addChild(winHandBtn);
        
        winHandBtn->setOnClickListener([=](Node*) {
            SB_SAFE_PERFORM_LISTENER(this, onExitAppListener);
        });
        
        auto scale1 = ScaleTo::create(0.5f, 1.1f);
        auto scale2 = ScaleTo::create(0.5f, 1.0f);
        auto seq = Sequence::create(scale1, scale2, nullptr);
        winHandBtn->runAction(RepeatForever::create(seq));
        
        // Lose Hand
//        auto loseHandSpr = SBEffectSprite::create(DIR_IMG_GAME + getFile(getLoseHand(myHand)));
//        auto loseHandBtn = SBButton::create(DIR_IMG_GAME + getFile(getLoseHand(myHand)));
        auto loseHandBtn = Widget::create();
        loseHandBtn->setTouchEnabled(true);
        loseHandBtn->setAnchorPoint(ANCHOR_M);
        loseHandBtn->setPosition(POS[1]);
        stoneBg->addChild(loseHandBtn);
        
        // sprite
        {
            auto spr = superbomb::EffectSprite::create(DIR_IMG_GAME + getFile(getLoseHand(myHand)));
            spr->setAnchorPoint(ANCHOR_M);
            spr->setPosition(Vec2MC(spr->getContentSize(), 0, 0));
//            spr->setEffect(superbomb::Effect::create("shaders/example_GreyScale.fsh"));
            loseHandBtn->addChild(spr);

            loseHandBtn->setContentSize(spr->getContentSize());
        }
        
        // 일반 투명
        /*
        {
            auto stencil = superbomb::EffectSprite::create(DIR_IMG_GAME + getFile(getLoseHand(myHand)));
            stencil->setAnchorPoint(Vec2::ZERO);
            stencil->setPosition(Vec2::ZERO);
            
            auto spr = superbomb::EffectSprite::create(DIR_IMG_GAME + getFile(getLoseHand(myHand)));
            
            loseHandBtn->setContentSize(spr->getContentSize());
            
            auto clippingNode = ClippingNode::create(stencil);
            clippingNode->setAnchorPoint(ANCHOR_M);
            clippingNode->setPosition(Vec2MC(spr->getContentSize(), 0, 0));
            clippingNode->setContentSize(spr->getContentSize());
            clippingNode->setAlphaThreshold(0);
//            clippingNode->setInverted(true);
            loseHandBtn->addChild(clippingNode);
            
            spr->setAnchorPoint(ANCHOR_M);
            spr->setPosition(Vec2MC(spr->getContentSize(), 0, 0));
            clippingNode->addChild(spr);

            clippingNode->addChild(SBNodeUtils::createBackgroundNode(clippingNode, Color4B(0,0,0,255*0.5f)));
        }
        */
        
        loseHandBtn->addClickEventListener([=](Ref*) {
            this->dismissWithAction();
        });
        
//        loseHandBtn->setOnClickListener([=](Node*) {
//            this->dismissWithAction();
//        });
    }
}

/**
 * 등장 연출
 */
void ExitAlertPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(onFinished);
    
    runBackgroundFadeInAction(nullptr, FADE_DURATION);      // 배경 fade in
    runSlideInAction([=]() {                                // 컨텐츠 slide in
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 퇴장 연출
 */
void ExitAlertPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(onFinished);
    
    runBackgroundFadeOutAction(nullptr, FADE_DURATION);     // 배경 fade out
    runSlideOutAction([=]() {                               // 컨텐츠 slide out
        
        this->onExitActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_DURATION);
}

/**
 * 등장 연출 완료
 */
void ExitAlertPopup::onEnterActionFinished() {
    
    // 비석 바깥 영역 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->dismissWithAction();
        // this->performListener(Tag::RESUME);
    });
    
    // auto box = SBNodeUtils::getBoundingBoxInWorld(stoneBg);
    
    auto stoneTouchNode = SBNodeUtils::createTouchNode();
    stoneTouchNode->setAnchorPoint(stoneBg->getAnchorPoint());
    stoneTouchNode->setPosition(stoneBg->getPosition());
    stoneTouchNode->setContentSize(stoneBg->getContentSize());
    addChild(stoneTouchNode);
}

