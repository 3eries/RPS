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

static const string SCHEDULER_TOUCH_LOCKED = "SCHEDULER_TOUCH_LOCKED";

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
    initTapHint();
    
    GameManager::getInstance()->addListener(this);
    
    switchButton(GameMode::NORMAL);
    
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

/**
 * 버튼 터치 활성화 설정
 */
void RSPButtonLayer::setButtonTouchEnabled(bool enabled) {
    
    for( auto btn : buttons ) {
        btn->setTouchEnabled(enabled);
    }
}

/**
 * 터치 잠금
 * @param duration 잠금 시간
 */
void RSPButtonLayer::touchLocked(float duration) {
    
    setButtonTouchEnabled(false);
    
    if( duration > 0 ) {
        scheduleOnce([=](float dt) {
            this->setButtonTouchEnabled(true);
        }, duration, SCHEDULER_TOUCH_LOCKED);
    }
}

/**
 * TAP 힌트 노출
 */
void RSPButtonLayer::showTapHint(RSPType winHand) {
    
    auto getButton = [=]() -> Node* {
        for( auto btn : buttons ) {
            if( btn->getType() == winHand ) {
                return btn;
            }
        }
        
        return nullptr;
    };
    
    auto btn = getButton();
    CCASSERT(btn != nullptr, "RSPButtonLayer::showTapHint error.");
    
    auto btnBox = SBNodeUtils::getBoundingBoxInWorld(btn);
    
    // 초기 좌표 설정
    tapHint->setVisible(true);
    tapHint->setPosition(Vec2(btnBox.getMidX(), btnBox.getMidY()+50));
    
    // Tap 액션
    auto move1 = MoveTo::create(0.5f, tapHint->getPosition());
    auto move2 = MoveTo::create(0.5f, tapHint->getPosition() + Vec2(0, 20));
    auto seq = Sequence::create(move1, move2, nullptr);
    
    tapHint->runAction(RepeatForever::create(seq));
}

void RSPButtonLayer::hideTapHint() {
    
    tapHint->setVisible(false);
    tapHint->stopAllActions();
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
            
            this->hideTapHint();
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
            
            this->hideTapHint();
            onFeverButtonClickListener(i);
        });
    }
    
    // 피버 모드 2번째(우측) 버튼 flip
    auto btn = buttons[buttons.size()-1];
    btn->getImage()->setFlippedX(true);
}

/**
 * TAP 힌트 초기화
 */
void RSPButtonLayer::initTapHint() {
    
    tapHint = LayerColor::create(Color4B(0,0,0,255*0.7f));
    tapHint->setIgnoreAnchorPointForPosition(false);
    tapHint->setVisible(false);
    tapHint->setAnchorPoint(ANCHOR_M);
    tapHint->setContentSize(Size(130, 70));
    addChild(tapHint);
    
    auto label = Label::createWithTTF("TAP", FONT_RETRO, 50);
    label->setAnchorPoint(ANCHOR_M);
    label->setPosition(Vec2MC(tapHint->getContentSize(), 0, 0));
    label->setColor(Color3B(255,255,255));
    tapHint->addChild(label);
}
