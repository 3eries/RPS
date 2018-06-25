//
//  UIHelper.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// 버튼 정의
const Size ButtonSize::SMALL              = Size(200, 80);
const Size ButtonSize::MEDIUM             = Size(250, 80);
const Size ButtonSize::LARGE              = Size(300, 80);

string UIHelper::getButtonImageFile(RSPType type) {
    
    switch( type ) {
        case RSPType::ROCK:          return DIR_IMG_GAME + "RSP_btn_rock.png";
        case RSPType::SCISSORS:      return DIR_IMG_GAME + "RSP_btn_scissors.png";
        case RSPType::PAPER:         return DIR_IMG_GAME + "RSP_btn_paper.png";
        case RSPType::ROCK_N_ROLL:   return DIR_IMG_GAME + "RSP_btn_fever.png";
        default:
            CCASSERT(false, "UIHelper::getButtonImageFile error: invalid rsp type.");
            break;
    }
    
    return "";
}

Color3B UIHelper::getColor(RSPType type) {
    
    switch( type ) {
        case RSPType::ROCK:        return Color3B(255, 0, 0);
        case RSPType::SCISSORS:    return Color3B(0, 255, 0);
        case RSPType::PAPER:       return Color3B(0, 0, 255);
        default:
            CCASSERT(false, "UIHelper::getColor error: invalid rsp type.");
            break;
    }
    
    return Color3B::WHITE;
}

/**
 * 기본 버튼 생성
 */
SBButton* UIHelper::createButton(SBButton::Config config) {
    
    auto btn = SBButton::create(config);
    btn->setZoomScale(0.1f);
    return btn;
}

/**
 * 폰트 버튼 생성
 */
SBButton* UIHelper::createFontButton(const string &title, const Size &size) {
    
    auto btn = createButton(SBButton::Config("", size, title, FONT_RETRO, 50));
    
    auto titleLabel = btn->getTitle();
    titleLabel->setColor(Color3B::WHITE);
    titleLabel->enableOutline(Color4B::BLACK, 3);
    
    return btn;
}
