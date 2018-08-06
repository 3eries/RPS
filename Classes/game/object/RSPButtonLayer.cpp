//
//  RSPButtonLayer.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 5..
//

#include "RSPButtonLayer.hpp"

#include "RSPButton.hpp"

USING_NS_CC;
using namespace spine;
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
    
    CCASSERT(buttons.size() > 0, "RSPButtonLayer::showTapHint error: invalid button list");
    
    hideTapHint(false);
    
    // 첫번째 버튼 기준으로 애니메이션 재생
    auto firstBtn = buttons[0];
    string animName;
    
    switch( firstBtn->getType() ) {
        case RSPType::ROCK:          animName = "tap_normal_left";          break;
        case RSPType::PAPER:         animName = "tap_normal_center";        break;
        case RSPType::SCISSORS:      animName = "tap_normal_right";         break;
        case RSPType::ROCK_N_ROLL:   animName = "tap_fever";                break;
        default:
            CCASSERT(false, "RSPButtonLayer::showTapHint error: invalid rsp type.");
            break;
    }
    
    if( IS_IPAD ) {
        animName += "_ipad";
    }
    
    tapHint->setVisible(true);
    tapHint->setAnimation(0, animName, true);
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

    tapHint->stopAllActions();
//    tapHint->setScale(1);
    
    auto hide = [=]() {
        tapHint->setVisible(false);
        tapHint->setOpacity(255);
        SBSpineHelper::clearAnimation(tapHint, ANIM_NAME_CLEAR);
    };
    
    if( !runAction ) {
        hide();
        return;
    }
    
    // scale out
    /*
    auto scaleOut = ScaleTo::create(0.1f, 1, 0);
    auto callFunc = CallFunc::create(hide);
    tapHint->runAction(Sequence::create(scaleOut, callFunc, nullptr));
     */
    // fade out
    tapHint->setOpacity(255);
    
    auto fadeOut = FadeOut::create(0.1f);
    auto callFunc = CallFunc::create(hide);
    tapHint->runAction(Sequence::create(fadeOut, callFunc, nullptr));
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
    
    RSPType types[] = {
        RSPType::ROCK,
        RSPType::PAPER,
        RSPType::SCISSORS,
    };
    const float POS_Y = 100;
    
    SBUIInfo uiInfos[] = {
        SBUIInfo(ANCHOR_ML, Vec2BL(164 -  (304*0.5f), POS_Y),    ""),
        SBUIInfo(ANCHOR_M,  Vec2BC(0,                 POS_Y),    ""),
        SBUIInfo(ANCHOR_MR, Vec2BR(-164 + (304*0.5f), POS_Y),    ""),
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
    
    const RSPType type = RSPType::ROCK_N_ROLL;
    const float POS_Y = 100;

    SBUIInfo uiInfos[] = {
        SBUIInfo(ANCHOR_ML, Vec2BL(243 -  (466*0.5f), POS_Y),    ""),
        SBUIInfo(ANCHOR_MR, Vec2BR(-243 + (466*0.5f), POS_Y),    ""),
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
    
    tapHint = SkeletonAnimation::createWithJsonFile(DIR_ANIM + "tap.json");
    tapHint->setAnchorPoint(Vec2::ZERO);
    tapHint->setPosition(Vec2MC(0, 0));
    addChild(tapHint);
    
    tapHint->setVisible(false);
    SBSpineHelper::clearAnimation(tapHint, ANIM_NAME_CLEAR);
}
