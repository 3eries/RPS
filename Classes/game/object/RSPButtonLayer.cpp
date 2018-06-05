//
//  RSPButtonLayer.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 5..
//

#include "RSPButtonLayer.hpp"

#include "RSPButton.hpp"

USING_NS_CC;
using namespace std;

RSPButtonLayer::RSPButtonLayer() :
onNormalButtonClickListener(nullptr),
onFeverButtonClickListener(nullptr) {
}

RSPButtonLayer::~RSPButtonLayer(){
}

bool RSPButtonLayer::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initNormalButtons();
    initFeverButtons();
    
    GameManager::getInstance()->addListener(this);
    
    return true;
}

void RSPButtonLayer::onExit() {
    
    GameManager::getInstance()->removeListener(this);
    
    Node::onExit();
}

void RSPButtonLayer::onGameStart() {
    
    switchButton(GameMode::NORMAL);
}

/**
 * 게임 모드 전환
 */
void RSPButtonLayer::onGameModeChanged(GameMode mode) {
    
    switch( mode ) {
        case GameMode::NORMAL: {
        } break;
            
        case GameMode::FEVER: {
            switchButton(mode);
        } break;
    }
}

/**
 * 게임 모드에 따른 버튼 전환
 */
void RSPButtonLayer::switchButton(GameMode mode) {
    
    switch( mode ) {
        case GameMode::NORMAL: {
            buttonLayers[GameMode::NORMAL]->setVisible(true);
            buttonLayers[GameMode::FEVER]->setVisible(false);
            
        } break;
            
        case GameMode::FEVER: {
            buttonLayers[GameMode::NORMAL]->setVisible(false);
            buttonLayers[GameMode::FEVER]->setVisible(true);
            
        } break;
    }
}

void RSPButtonLayer::setButtonTouchEnabled(bool enabled) {
    
    for( auto btn : buttons ) {
        btn->setTouchEnabled(enabled);
    }
}

/**
 * 노멀 모드 버튼 초기화
 */
void RSPButtonLayer::initNormalButtons() {
    
    auto layer = SBNodeUtils::createWinSizeNode();
    addChild(layer);
    
    buttonLayers[GameMode::NORMAL] = layer;
    
    const float POS_Y = 5;
    
    RSPType types[] = {
        RSPType::ROCK,
        RSPType::SCISSORS,
        RSPType::PAPER,
    };
    
    SBUIInfo uiInfos[] = {
        SBUIInfo(ANCHOR_BL, Vec2BL(20,   POS_Y),    ""),
        SBUIInfo(ANCHOR_MB, Vec2BC(0,    POS_Y),    ""),
        SBUIInfo(ANCHOR_BR, Vec2BR(-20,  POS_Y),    ""),
    };
    
    for( int i = 0; i < sizeof(types) / sizeof(RSPType); ++i ) {
        auto type = types[i];
        
        auto btn = RSPButton::create(type);
        uiInfos[i].apply(btn);
        layer->addChild(btn);
        
        buttons.push_back(btn);
        
        btn->setOnClickListener([=](Node*) {
            onNormalButtonClickListener(type);
        });
    }
}

/**
 * 피버 모드 버튼 초기화
 */
void RSPButtonLayer::initFeverButtons() {

    auto layer = SBNodeUtils::createWinSizeNode();
    addChild(layer);
    
    buttonLayers[GameMode::FEVER] = layer;
    
    const float POS_Y = 5;
    const RSPType type = RSPType::ROCK_N_ROLL;
    
    SBUIInfo uiInfos[] = {
        SBUIInfo(ANCHOR_BR, Vec2BC(-5,   POS_Y),    ""),
        SBUIInfo(ANCHOR_BL, Vec2BC(5,    POS_Y),    ""),
    };
    
    for( int i = 0; i < sizeof(uiInfos) / sizeof(SBUIInfo); ++i ) {
        auto btn = RSPButton::create(type);
        uiInfos[i].apply(btn);
        layer->addChild(btn);
        
        buttons.push_back(btn);
        
        btn->setOnClickListener([=](Node*) {
            onFeverButtonClickListener(i);
        });
    }
}
