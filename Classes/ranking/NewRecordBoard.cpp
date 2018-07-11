//
//  NewRecordBoard.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 5..
//

#include "NewRecordBoard.hpp"

#include "RSP.h"
#include "UserDefaultKey.h"

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

void NewRecordBoard::setInitial(int nameIndex, const string &initial) {
    
    for( int i = 0; i < INITIAL_LIST.size(); ++i ) {
        if( INITIAL_LIST[i] == initial ) {
            setInitial(nameIndex, i);
            return;
        }
    }
    
    CCASSERT(false, "NewRecordBoard::setInitial(int, string) error");
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
    // RSP_popup_bg_new_record.png Vec2MC(0, 0) , Size(696, 536)
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_new_record.png");
    bg->setAnchorPoint(Vec2::ZERO);
    bg->setPosition(Vec2::ZERO);
    addChild(bg);
    
    const Size bgSize(bg->getContentSize());
    setContentSize(bgSize);
    
    // image
    // RSP_popup_title_new_record.png Vec2MC(-225, 188) , Size(166, 96)
    // RSP_popup_bg_name.png Vec2MC(-84, -70) , Size(440, 244)
    SBUIInfo infos[] = {
        SBUIInfo(ANCHOR_M, Vec2MC(bgSize, -225, 188), "RSP_popup_title_new_record.png"),   // title
        // SBUIInfo(ANCHOR_M, Vec2MC(bgSize, -84, -70),  "RSP_popup_bg_name.png"),            // name bg
    };
    
    for( int i = 0; i < sizeof(infos)/sizeof(SBUIInfo); ++i ) {
        auto info = infos[i];
        
        auto spr = Sprite::create(DIR_IMG_GAME + info.file);
        info.apply(spr);
        addChild(spr);
    }
    
    // score
    // 186,0,0 sabo size:122 Vec2MC(106, 134) , Size(318, 90)
    // 1234 Vec2MC(22, 191) , Size(267, 77)
    // 저 빨간 점수 1234는 그림자가 위, 아래로 들어가서 두께감을 주고 싶은데
    // 위쪽 그림자는 4px, 컬러는 99,0,0 이고,
    // 아래쪽 하얀 그림자는 4px, 컬러는 240, 243,243입니당
    //
    // ranking2.psd 에서 레코드 팝업 폰트 사이즈는 노랑색 이니셜은 예전이랑 동일하고, 빨간 스코어 는 108포인트네용
    scoreLabel = Label::createWithTTF("", FONT_NEW_RECORD, 108,
                                      Size(0, 0), TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setColor(Color3B(186,0,0));
    scoreLabel->enableShadow(Color4B(240, 243, 243, 255), Size(4, -4), 10);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MC(bgSize, 22, 191));
    addChild(scoreLabel);
}

/**
 * 이니셜 초기화
 */
void NewRecordBoard::initInitial() {
 
    const Size bgSize(getContentSize());
    
    const float POS_X[] = {
        Vec2MC(bgSize, -196, 0).x,
        Vec2MC(bgSize, 0, 0).x,
        Vec2MC(bgSize, 196, 0).x,
    };
    
    // 화살표 버튼 배경
    // RSP_btn_name_control.png Vec2MC(-196, -106) , Size(168, 228)
    // RSP_btn_name_control.png Vec2MC(0, -106) , Size(168, 228)
    // RSP_btn_name_control.png Vec2MC(196, -106) , Size(168, 228)
    vector<Sprite*> arrowBtnBackgrounds;
    
    {
        const float POS_Y = Vec2MC(bgSize, 0, -106).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto bg = Sprite::create(DIR_IMG_GAME + "RSP_btn_name_control.png");
            bg->setAnchorPoint(ANCHOR_M);
            bg->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(bg);
            
            arrowBtnBackgrounds.push_back(bg);
        }
    }
    
    // 화살표 버튼 생성
    const Size ARROW_BTN_SIZE(196, 126);
    
    auto createArrowButton = [=](int i) -> Widget* {
      
        auto btn = Widget::create();
        btn->setAnchorPoint(ANCHOR_M);
        btn->setContentSize(ARROW_BTN_SIZE);
        btn->setTouchEnabled(true);
        
        // btn->addChild(SBNodeUtils::createBackgroundNode(btn, Color4B(255,0,0,255*0.3f)));
        
        btn->addTouchEventListener([=](Ref*, Widget::TouchEventType eventType) {
            
            switch( eventType ) {
                case Widget::TouchEventType::BEGAN: {
                    arrowBtnBackgrounds[i]->setScale(1.04f);
                } break;
                    
                case Widget::TouchEventType::ENDED:
                case Widget::TouchEventType::CANCELED: {
                    arrowBtnBackgrounds[i]->setScale(1.0f);
                } break;
                    
                default: break;
            }
        });
        
        return btn;
    };
    
    // up 버튼
    // touch Vec2MC(-196, -42) , Size(196, 126)
    // touch Vec2MC(0, -42) , Size(196, 126)
    // touch Vec2MC(196, -42) , Size(196, 126)
    {
        const float POS_Y = Vec2MC(bgSize, 0, -42).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto btn = createArrowButton(i);
            btn->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(btn);
            
            btn->addClickEventListener([=](Ref*) {
                this->initialUp(i);
            });
        }
    }
    
    // down 버튼
    // touch Vec2MC(-196, -168) , Size(196, 126)
    // touch Vec2MC(0, -168) , Size(196, 126)
    // touch Vec2MC(196, -168) , Size(196, 126)
    {
        const float POS_Y = Vec2MC(bgSize, 0, -168).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto btn = createArrowButton(i);
            btn->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(btn);
            
            btn->addClickEventListener([=](Ref*) {
                this->initialDown(i);
            });
        }
    }
    
    // 이니셜
    // 최초 이니셜은 마지막으로 등록된 이름으로 설정
    // S Vec2MC(-195, 67) , Size(63, 76)
    // S Vec2MC(0, 66) , Size(63, 76)
    // W Vec2MC(196, 66) , Size(71, 76)
    string lastRecordName = UserDefault::getInstance()->getStringForKey(UserDefaultKey::LAST_RECORD_NAME, "");
    vector<string> lastRecordNameChars;
    
    if( lastRecordName != "" ) {
        lastRecordNameChars = SBStringUtils::toArray(lastRecordName);
    } else {
        for( int i = 0; i < 3; ++i ) {
            lastRecordNameChars.push_back(INITIAL_LIST[0]);
        }
    }
    
    {
        const float POS_Y = Vec2MC(bgSize, 0, 66).y;
        
        for( int i = 0; i < 3; ++i ) {
            auto label = Label::createWithTTF("", FONT_NEW_RECORD, 105);
            label->setColor(Color3B(255,217,0));
            label->setAnchorPoint(ANCHOR_M);
            label->setPosition(Vec2(POS_X[i], POS_Y));
            addChild(label);
            
            NameInitial nameInitial;
            nameInitial.label = label;
            nameInitials.push_back(nameInitial);
            
            setInitial(i, lastRecordNameChars[i]);
        }
    }
    
    // 엔터 버튼
    // RSP_btn_name_enter.png Vec2MC(252, 190) , Size(136, 100)
    enterBtn = SBButton::create(DIR_IMG_GAME + "RSP_btn_name_enter.png");
    enterBtn->setZoomScale(0.05f);
    enterBtn->setAnchorPoint(ANCHOR_M);
    enterBtn->setPosition(Vec2MC(bgSize, 252, 190));
    addChild(enterBtn);
    
    enterBtn->setOnClickListener([=](Node*) {
        
        this->retain();
        onInputCompletedListener(this->getName());
        this->release();
    });
}
