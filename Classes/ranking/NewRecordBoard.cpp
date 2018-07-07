//
//  NewRecordBoard.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 5..
//

#include "NewRecordBoard.hpp"

#include "RSP.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const float NewRecordBoard::EXIT_DURATION = 0.1f;

static const vector<string> INITIAL_LIST = {
    "A", "B", "C", "D", "E", "F", "G", "H",
    "I", "J", "K", "L", "M", "N", "O", "P",
    "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
};

NewRecordBoard* NewRecordBoard::create(int score) {
 
    auto board = new NewRecordBoard(score);
    
    if( board && board->init() ) {
        board->autorelease();
        return board;
    }
    
    delete board;
    return nullptr;
}

NewRecordBoard::NewRecordBoard(int score) :
score(score),
onNameChangedListener(nullptr),
onInputCompletedListener(nullptr) {
    
}

NewRecordBoard::~NewRecordBoard() {
}

bool NewRecordBoard::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    initBg();
    initInitial();
    
    return true;
}

/**
 * 이니셜 설정
 */
void NewRecordBoard::setInitial(int nameIndex, int initialIndex) {
    
    if( initialIndex < 0 ) {
        initialIndex = INITIAL_LIST.size()-1;
    }
    
    if( initialIndex > INITIAL_LIST.size()-1 ) {
        initialIndex = 0;
    }
    
    auto &nameInitial = nameInitials[nameIndex];
    nameInitial.str = INITIAL_LIST[initialIndex];
    nameInitial.initialIndex = initialIndex;
    nameInitial.label->setString(nameInitial.str);
    
    if( onNameChangedListener ) {
        onNameChangedListener(getName());
    }
}

void NewRecordBoard::initialUp(int nameIndex) {
    
    setInitial(nameIndex, nameInitials[nameIndex].initialIndex+1);
}

void NewRecordBoard::initialDown(int nameIndex) {
    
    setInitial(nameIndex, nameInitials[nameIndex].initialIndex-1);
}

/**
 * 이름 반환
 */
string NewRecordBoard::getName() {
    
    string name = "";
    
    for( auto nameInitial : nameInitials ) {
        name += nameInitial.str;
    }
    
    return name;
}

/**
 * 등장 연출
 */
void NewRecordBoard::runEnterAction(float duration, SBCallback onFinished) {
    
    Vec2 originPos = getPosition();
    setPosition(Vec2(originPos.x, -getContentSize().height*0.5f));
    
    auto move = MoveTo::create(duration, originPos);
    auto callFunc = CallFunc::create([=]() {
        
        this->runScoreAction();
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(move, callFunc, nullptr));
}

/**
 * 퇴장 연출
 */
void NewRecordBoard::runExitAction(SBCallback onFinished) {
    
    enterBtn->setTouchEnabled(false);
    
    auto move = MoveTo::create(EXIT_DURATION, Vec2(getPosition().x, -getContentSize().height*0.5f));
    auto callFunc = CallFunc::create([=]() {
        SB_SAFE_PERFORM_LISTENER(this, onFinished);
    });
    runAction(Sequence::create(move, callFunc, nullptr));
}

/**
 * 스코어 연출
 */
void NewRecordBoard::runScoreAction() {
    
    /*
     if( score < 10 ) {
     scoreLabel->setString(TO_STRING(score));
     return;
     }
     */
    
    auto rolling = SBRollingNumber::create([=](string number) {
        scoreLabel->setString(number);
    }, score, 0.15f);
    
    scoreLabel->runAction(rolling);
    
    // debug rolling
    /*
     auto delay = DelayTime::create(4.0f);
     auto callFunc = CallFunc::create([=]() {
     MessageBox("rolling finished", "");
     });
     runAction(Sequence::create(delay, callFunc, nullptr));
     */
}

/**
 * 배경 초기화
 */
void NewRecordBoard::initBg() {
    
    // bg
    // RSP_popup_bg_new_record.png Vec2MC(0, 0) , Size(696, 464)
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_new_record.png");
    bg->setAnchorPoint(Vec2::ZERO);
    bg->setPosition(Vec2::ZERO);
    addChild(bg);
    
    const Size bgSize(bg->getContentSize());
    setContentSize(bgSize);
    
    // image
    // RSP_popup_title_new_record.png Vec2MC(-200, 130) , Size(184, 108)
    // RSP_popup_bg_name.png Vec2MC(-84, -70) , Size(440, 244)
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2MC(bgSize, -200, 130), "RSP_popup_title_new_record.png"),   // title
        SBUIInfo(ANCHOR_M, Vec2MC(bgSize, -84, -70),  "RSP_popup_bg_name.png"),            // name bg
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(DIR_IMG_GAME + info.file);
        info.apply(spr);
        addChild(spr);
    }
    
    // score
    // 186,0,0 sabo size:122 Vec2MC(106, 134) , Size(318, 90)
    // 저 빨간 점수 1234는 그림자가 위, 아래로 들어가서 두께감을 주고 싶은데
    // 위쪽 그림자는 4px, 컬러는 99,0,0 이고,
    // 아래쪽 하얀 그림자는 4px, 컬러는 240, 243,243입니당
    scoreLabel = Label::createWithTTF("", FONT_NEW_RECORD, 122,
                                      Size(0, 0), TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setColor(Color3B(186,0,0));
    scoreLabel->enableShadow(Color4B(240, 243, 243, 255), Size(4, -4), 10);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(bgSize, 106, 134));
    addChild(scoreLabel);
}

/**
 * 이니셜 초기화
 */
void NewRecordBoard::initInitial() {
 
    const Size bgSize(getContentSize());
    
    const float POS_X[] = {
        Vec2MC(bgSize, -228, 0).x,
        Vec2MC(bgSize, -84, 0).x,
        Vec2MC(bgSize, 61, 0).x,
    };
    
    // up 버튼
    // RSP_btn_name_up.png Vec2MC(-228, 3) , Size(64, 36)
    // RSP_btn_name_up.png Vec2MC(-84, 3) , Size(64, 36)
    // RSP_btn_name_up.png Vec2MC(61, 3) , Size(64, 36)
    {
        const float POS_Y = Vec2MC(bgSize, 0, 3).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto btn = SBButton::create(DIR_IMG_GAME + "RSP_btn_name_up.png");
            btn->setZoomScale(0.2f);
            btn->setAnchorPoint(ANCHOR_M);
            btn->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(btn);
            
            btn->setOnClickListener([=](Node*) {
                this->initialUp(i);
            });
        }
    }
    
    // down 버튼
    // RSP_btn_name_down.png Vec2MC(-228, -148) , Size(64, 36)
    // RSP_btn_name_down.png Vec2MC(-84, -148) , Size(64, 36)
    // RSP_btn_name_down.png Vec2MC(61, -148) , Size(64, 36)
    {
        const float POS_Y = Vec2MC(bgSize, 0, -148).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto btn = SBButton::create(DIR_IMG_GAME + "RSP_btn_name_down.png");
            btn->setZoomScale(0.2f);
            btn->setAnchorPoint(ANCHOR_M);
            btn->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(btn);
            
            btn->setOnClickListener([=](Node*) {
                this->initialDown(i);
            });
        }
    }
    
    // 이니셜
    // 255,217,0 sabo size:105 Vec2MC(-228, -70) , Size(62, 71)
    // 255,217,0 sabo size:105 Vec2MC(-84, -70) , Size(62, 71)
    // 255,217,0 sabo size:105 Vec2MC(62, -70) , Size(70, 71)
    {
        const float POS_Y = Vec2MC(bgSize, 0, -70).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto label = Label::createWithTTF("", FONT_NEW_RECORD, 105);
            label->setColor(Color3B(255,217,0));
            label->setAnchorPoint(ANCHOR_M);
            label->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(label);
            
            NameInitial nameInitial;
            nameInitial.label = label;
            nameInitials.push_back(nameInitial);
            
            setInitial(i, 0);
        }
    }
    
    // 엔터 버튼
    // RSP_btn_name_enter.png Vec2MC(235, -70) , Size(144, 248)
    enterBtn = SBButton::create(DIR_IMG_GAME + "RSP_btn_name_enter.png");
    enterBtn->setZoomScale(0.07f);
    enterBtn->setAnchorPoint(ANCHOR_M);
    enterBtn->setPosition(Vec2MC(bgSize, 235, -70));
    addChild(enterBtn);
    
    enterBtn->setOnClickListener([=](Node*) {
        
        this->retain();
        onInputCompletedListener(this->getName());
        this->release();
    });
}
