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
static const int    TAP_HINT_COUNT         = 2; // 탭 힌트 생성 갯 수

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
    
    feverButtonTouchCount = 0;
    
    switchButton(GameMode::NORMAL);
}

/**
 * 게임 오버 전
 */
void RSPButtonLayer::onPreGameOver() {
    
    hideTapHint(false);
}

/**
 * 이어하기
 */
void RSPButtonLayer::onContinue() {
}

/**
 * 게임 오버
 */
void RSPButtonLayer::onGameOver() {
}

/**
 * 게임 모드 전환
 */
void RSPButtonLayer::onGameModeChanged(GameMode mode) {
    
    feverButtonTouchCount = 0;
    
    switch( mode ) {
        case GameMode::NORMAL: {
        } break;
            
        case GameMode::FEVER: {
            switchButton(mode);
        } break;
    }
}

/**
 * 마지막 피버 블럭 히트
 */
void RSPButtonLayer::onLastFeverBlockHit() {
    
    hideTapHint();
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
void RSPButtonLayer::showTapHint(std::vector<RSPButton*> buttons) {
    
    hideTapHint(false);
    
    for( int i = 0; i < buttons.size(); ++i ) {
        if( i == tapHints.size() ) {
            CCLOG("RSPButtonLayer::showTapHint(std::vector<RSPButton*>) warning: index out");
            break;
        }
    
        // 검색된 버튼 기준으로 좌표 설정
        auto btn = buttons[i];
        auto btnBox = SBNodeUtils::getBoundingBoxInWorld(btn);
        
        auto tapHint = tapHints[i];
        tapHint->setVisible(true);
        tapHint->setPosition(Vec2(btnBox.getMidX(), btnBox.getMidY()+50));
    }
    
    tapHintLayer->setVisible(true);
    
    auto move1 = MoveTo::create(0.5f, Vec2(0, 20));
    auto move2 = MoveTo::create(0.5f, Vec2(0, 0));
    auto seq = Sequence::create(move1, move2, nullptr);
    
    tapHintLayer->runAction(RepeatForever::create(seq));
}

void RSPButtonLayer::showTapHint(RSPType winHand) {
    
    // 타입에 해당하는 버튼 검색
    auto getButton = [=]() -> RSPButton* {
        for( auto btn : buttons ) {
            if( btn->getType() == winHand ) {
                return btn;
            }
        }
        
        return nullptr;
    };
    
    auto btn = getButton();
    CCASSERT(btn != nullptr, "RSPButtonLayer::showTapHint(RSPType) error.");
    
    showTapHint(vector<RSPButton*>({btn}));
}

void RSPButtonLayer::showTapHintFeverMode() {
    
    showTapHint(vector<RSPButton*>({
        buttons[FEVER_MODE_BUTTON_INDEX], buttons[FEVER_MODE_BUTTON_INDEX+1]
    }));
}

/**
 * TAP 힌트 숨김
 */
void RSPButtonLayer::hideTapHint(bool runAction) {
    
    tapHintLayer->stopAllActions();

    auto hide = [=]() {
        for( auto tapHint : tapHints ) {
            tapHint->stopAllActions();
            tapHint->setScale(1);
            tapHint->setVisible(false);
        }
        
        tapHintLayer->setVisible(false);
        tapHintLayer->setPosition(Vec2::ZERO);
    };
    
    if( !runAction ) {
        hide();
        return;
    }
    
    // scale out
    const float duration = 0.1f;
    
    for( auto tapHint : tapHints ) {
        tapHint->stopAllActions();
        
        if( !tapHint->isVisible() ) {
            continue;
        }
        
        auto scaleOut = ScaleTo::create(0.1f, 1, 0);
        tapHint->runAction(scaleOut);
    }
    
    auto delay = DelayTime::create(duration*1.1f);
    auto callFunc = CallFunc::create(hide);
    tapHintLayer->runAction(Sequence::create(delay, callFunc, nullptr));
    
    // fade out
//    auto hide = [=]() {
//        for( auto tapHint : tapHints ) {
//            tapHint->setVisible(false);
//        }
//
//        tapHintLayer->setOpacity(255);
//        tapHintLayer->setVisible(false);
//    };
//
//    auto fadeOut = FadeOut::create(0.1f);
//    auto callFunc = CallFunc::create(hide);
//    tapHintLayer->runAction(Sequence::create(fadeOut, callFunc, nullptr));
    
    // no action
//    for( auto tapHint : tapHints ) {
//        tapHint->setVisible(false);
//    }
//
//    tapHintLayer->stopAllActions();
//    tapHintLayer->setVisible(false);
//    tapHintLayer->setPosition(Vec2::ZERO);
}

/**
 * RSP 버튼 클릭
 */
void RSPButtonLayer::onClickNormalButton(RSPType type) {
    
    hideTapHint();
    onNormalButtonClickListener(type);
}

/**
 * 피버 버튼 클릭
 */
void RSPButtonLayer::onClickFeverButton(int i) {
    
    feverButtonTouchCount++;
    
    if( feverButtonTouchCount == 5 ) {
        // hideTapHint();
    }
    
    onFeverButtonClickListener(i);
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
        SBUIInfo(ANCHOR_BL, Vec2BL(10,   POS_Y),    ""),
        SBUIInfo(ANCHOR_MB, Vec2BC(0,    POS_Y),    ""),
        SBUIInfo(ANCHOR_BR, Vec2BR(-10,  POS_Y),    ""),
    };
    
    for( int i = 0; i < sizeof(types) / sizeof(RSPType); ++i ) {
        auto type = types[i];
        
        auto btn = RSPButton::create(type);
        uiInfos[i].apply(btn);
        layer->addChild(btn);
        
        buttons.push_back(btn);
        
        btn->setOnClickListener([=](Node*) {
            this->onClickNormalButton(type);
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
            this->onClickFeverButton(i);
        });
    }
    
    // 피버 모드 1번째(좌측) 버튼 flip
    auto btn = buttons[FEVER_MODE_BUTTON_INDEX];
    btn->getImage()->setFlippedX(true);
}

/**
 * TAP 힌트 초기화
 */
void RSPButtonLayer::initTapHint() {
    
    auto createTapHint = []() -> Node* {
        auto tapHint = Node::create();
        tapHint->setCascadeOpacityEnabled(true);
        tapHint->setVisible(false);
        tapHint->setAnchorPoint(ANCHOR_M);
        // tapHint->setAnchorPoint(ANCHOR_MB);
        tapHint->setContentSize(Size(130, 70));
        
        auto bg = LayerColor::create(Color4B(0,0,0,255*0.7f));
        bg->setIgnoreAnchorPointForPosition(false);
        bg->setAnchorPoint(ANCHOR_M);
        bg->setPosition(Vec2MC(tapHint->getContentSize(), 0, 0));
        bg->setContentSize(tapHint->getContentSize());
        tapHint->addChild(bg);
        
        auto label = Label::createWithTTF("TAP", FONT_RETRO, 50);
        label->setAnchorPoint(ANCHOR_M);
        label->setPosition(Vec2MC(tapHint->getContentSize(), 0, 0));
        label->setColor(Color3B(255,255,255));
        tapHint->addChild(label);
        
        return tapHint;
    };
    
    tapHintLayer = SBNodeUtils::createZeroSizeNode();
    tapHintLayer->setCascadeOpacityEnabled(true);
    addChild(tapHintLayer);
    
    for( int i = 0; i < TAP_HINT_COUNT; ++i ) {
        auto tapHint = createTapHint();
        tapHintLayer->addChild(tapHint);
        
        tapHints.push_back(tapHint);
    }
}
