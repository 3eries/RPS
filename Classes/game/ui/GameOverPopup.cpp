//
//  GameOverPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "GameOverPopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"
#include "../GameManager.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

GameOverPopup::GameOverPopup() : SBBasePopup(),
contentLayer(nullptr),
onClickMenuListener(nullptr) {
    
}

GameOverPopup::~GameOverPopup() {
    
}

bool GameOverPopup::init() {
    
    if( !SBBasePopup::init() ) {
        return false;
    }
    
    initBg();
    initMenu();
    
    // 연출
    contentLayer->setPositionY(SB_WIN_SIZE.height);
    
    auto move = MoveTo::create(0.3f, Vec2(contentLayer->getPositionX(), 0));
    auto callFunc = CallFunc::create([=]() {
        // 배경음 재생
        SBAudioEngine::playBGM(SOUND_GAME_OVER, false);
    });
    contentLayer->runAction(Sequence::create(move, callFunc, nullptr));
    
    return true;
}

void GameOverPopup::onExit() {
    
    SBBasePopup::onExit();
}

void GameOverPopup::initBg() {
    
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
    auto titleLabel = Label::createWithTTF("GAME OVER", FONT_RETRO, 90);
    titleLabel->setAnchorPoint(ANCHOR_M);
    titleLabel->setPosition(Vec2MC(0, 400));
    titleLabel->setColor(Color3B::WHITE);
    titleLabel->enableOutline(Color4B::BLACK, 3);
    contentLayer->addChild(titleLabel);
    
    // 스코어 타이틀
    auto scoreTitleLabel = Label::createWithTTF("SCORE", FONT_RETRO, 80);
    scoreTitleLabel->setAnchorPoint(ANCHOR_M);
    scoreTitleLabel->setPosition(Vec2MC(0, 270));
    scoreTitleLabel->setColor(Color3B::WHITE);
    scoreTitleLabel->enableOutline(Color4B::BLACK, 3);
    contentLayer->addChild(scoreTitleLabel);
    
    // 스코어
    auto scoreBg = LayerColor::create(Color4B::WHITE);
    scoreBg->setIgnoreAnchorPointForPosition(false);
    scoreBg->setAnchorPoint(ANCHOR_M);
    scoreBg->setPosition(Vec2MC(0, 100));
    scoreBg->setContentSize(Size(500, 100));
    contentLayer->addChild(scoreBg);
    
    auto scoreLabel = Label::createWithTTF(TO_STRING(GameManager::getInstance()->getScore()),
                                           FONT_RETRO, 80);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(scoreBg->getContentSize(), 0, 0));
    scoreLabel->setColor(Color3B::BLACK);
    scoreBg->addChild(scoreLabel);
}

void GameOverPopup::initMenu() {
    
    auto menuLayer = SBNodeUtils::createZeroSizeNode();
    contentLayer->addChild(menuLayer);
    
    const Size MENU_SIZE(ButtonSize::LARGE);
    
    MenuType types[] = {
        MenuType::RESTART,
        MenuType::HOME,
    };
    
    string titles[] = {
        "RESTART",
        "HOME",
    };
    
    float posY = 0;
    float paddingY = 50;
    
    auto setLabelEffect = [](Label *label) {
        //        label->enableOutline(Color4B::BLACK, 3);
        //        label->enableShadow(Color4B(221, 6, 6, 255), Size(1,-1));
    };
    
    for( int i = 0; i < sizeof(types)/sizeof(MenuType); ++i ) {
        MenuType type = types[i];
        
        auto btn = UIHelper::createFontButton(titles[i], MENU_SIZE);
        btn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        btn->setPosition(Vec2(MENU_SIZE.width*0.5f, posY));
        menuLayer->addChild(btn);
        
        setLabelEffect(btn->getTitle());
        
        // 클릭 리스너
        btn->setOnClickListener([=](Node*) {
            
            if( onClickMenuListener ) {
                this->retain();
                onClickMenuListener(type);
                this->release();
            }
        });
        
        posY -= btn->getContentSize().height;
        posY -= paddingY;
    }
    
    Size size = SBNodeUtils::getChildrenBoundingSize(menuLayer);
    
    menuLayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    menuLayer->setPosition(Vec2MC(0, size.height-250));
    menuLayer->setContentSize(size);
}
