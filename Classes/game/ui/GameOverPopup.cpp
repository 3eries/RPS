//
//  GameOverPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameOverPopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"

#include "PopupManager.hpp"
#include "RankingManager.hpp"
#include "RecordRowView.hpp"

const float GameOverPopup::SLIDE_IN_DURATION               = 0.2f;
const float GameOverPopup::SLIDE_OUT_DURATION              = 0.2f;
const float GameOverPopup::FADE_IN_DURATION                = 0.5f;
const float GameOverPopup::FADE_OUT_DURATION               = 0.2f;

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

GameOverPopup* GameOverPopup::create(int score) {
    
    auto popup = new GameOverPopup(score);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

GameOverPopup::GameOverPopup(int score) : BasePopup(Type::GAME_OVER),
score(score) {
    
}

GameOverPopup::~GameOverPopup() {
    
}

bool GameOverPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    runEnterAction();
    
    return true;
}

void GameOverPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
}

void GameOverPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // RSP_popup_bg_big.png Vec2MC(0, 46) , Size(696, 956)
    stone = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_big.png");
    stone->setAnchorPoint(ANCHOR_M);
    stone->setPosition(Vec2MC(0, 46));
    addContentChild(stone);
    
    // images
    {
        // RSP_popup_title_rip.png Vec2MC(-4, 310) , Size(296, 108)
        // RSP_popup_text_score.png Vec2MC(0, 158) , Size(272, 60)
        // RSP_popup_text_top3.png Vec2MC(0, -72) , Size(216, 60)
        SBUIInfo infos[] = {
            SBUIInfo(ANCHOR_M,   Vec2MC(-4, 310),  "RSP_popup_title_rip.png"),      // rip
            SBUIInfo(ANCHOR_M,   Vec2MC(0, 158),   "RSP_popup_text_score.png"),     // score title
            SBUIInfo(ANCHOR_M,   Vec2MC(0, -72),   "RSP_popup_text_top3.png"),      // top3 title
        };
        
        for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
            auto info = infos[i];
            
            auto spr = Sprite::create(DIR_IMG_GAME + info.file);
            info.apply(spr);
            addContentChild(spr);
            
            fadeNodes.push_back(spr);
            
            /**
            if( i > 0 ) {
                spr->setScale(0.7f);
            }
             */
        }
    }
    
    // score
    {
        // RSP_popup_bg_score.png Vec2MC(-2, 66) , Size(420, 100)
        auto scoreBg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_score.png");
        scoreBg->setAnchorPoint(ANCHOR_M);
        scoreBg->setPosition(Vec2MC(-2, 66));
        addContentChild(scoreBg);
        
        fadeNodes.push_back(scoreBg);
        
        scoreLabel = Label::createWithTTF(TO_STRING(score), FONT_RETRO, 85);
        scoreLabel->setAnchorPoint(ANCHOR_M);
        scoreLabel->setPosition(Vec2MC(scoreBg->getContentSize(), 0, 0));
        scoreLabel->setColor(Color3B(255,255,0));
        scoreBg->addChild(scoreLabel);
    }
    
//    노랑글씨 크기는 85 컬러는 (rgb:255,255,0)
//    탑 3 흰글씨는 컬러는 완전 화이트, 크기는 50
//    폰트 종류는 둘다 commodore입니당
    
    // top3
    {
        // 1.BSJ - 1500 Vec2MC(-1, -138) , Size(376, 34)
        // 2.BSJ - 1500 Vec2MC(-1, -194) , Size(376, 34)
        // 3.BSJ - 1500 Vec2MC(-1, -255) , Size(376, 34)
        float posY[] = { -138, -194, -255 };
        
        auto records = RankingManager::getTopRecords(3);
        
        for( int i = 0; i < records.size(); ++i ) {
            auto record = records[i];
            
            auto rowView = RecordRowView::create(record);
            rowView->setPosition(Vec2MC(0, posY[i]));
            addContentChild(rowView);
            
            fadeNodes.push_back(rowView);
            
            // 달성 기록 하이라이트
            if( record.score == score ) {
                rowView->changeToHighlight();
            }
        }
    }
}

/**
 * 등장 연출
 */
void GameOverPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(onFinished);
    
    const bool existOtherPopup = PopupManager::getInstance()->exists(BasePopup::Type::NEW_RECORD);
    
    // 터치 잠금
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    // 액션 완료
    auto onActionFinished = [=]() {
        CCLOG("GameOverPopup::runEnterAction onActionFinished");
        
        // 터치 잠금 해제
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        // 배경음
        if( !existOtherPopup ) {
            SBAudioEngine::playBGM(SOUND_GAME_OVER, false);
        } else {
            SBAudioEngine::stopBGM();
        }
        
        this->onEnterActionFinished();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    };
    
    // 다른 팝업 있음, fade in
    if( existOtherPopup ) {
        // 연출 없음
        onActionFinished();
        
        // fade in
        /*
        for( auto n : fadeNodes ) {
            SBNodeUtils::recursiveCascadeOpacityEnabled(n, true);
            
            n->setOpacity(0);
            n->runAction(FadeIn::create(FADE_IN_DURATION));
        }
        
        auto delay = DelayTime::create(FADE_IN_DURATION * 1.05);
        auto callFunc = CallFunc::create(onActionFinished);
        runAction(Sequence::create(delay, callFunc, nullptr));
         */
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
    }
}

/**
 * 퇴장 연출
 */
void GameOverPopup::runExitAction(SBCallback onFinished) {
    
    BasePopup::runExitAction(onFinished);
    
    /*
    SBDirector::getInstance()->setScreenTouchLocked(true);
    
    runSlideOutAction([=]() {
        
        SBDirector::getInstance()->setScreenTouchLocked(false);
        this->onExitActionFinished();
        
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
        
    }, SLIDE_OUT_DURATION);
    */
    
    // fade out
    SBNodeUtils::recursiveCascadeOpacityEnabled(this, true);
    // SBDirector::getInstance()->setScreenTouchLocked(true);
    
    auto fade = FadeOut::create(FADE_OUT_DURATION);
    auto callFunc = CallFunc::create([=]() {
        
        CCLOG("GameOverPopup::runExitAction finished");
        
        // SBDirector::getInstance()->setScreenTouchLocked(false);
        this->onExitActionFinished();
        
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(fade, callFunc, nullptr));
}

/**
 * 등장 연출 완료
 */
void GameOverPopup::onEnterActionFinished() {
}

