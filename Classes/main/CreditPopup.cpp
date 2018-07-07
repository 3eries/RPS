//
//  CreditPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#include "CreditPopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// Member
struct Member {
    string image;
    string name;
    string job;
    
    Member(string _image, string _name, string _job) :
    image(_image), name(_name), job(_job) {}
};

// ART PROGRAMMING DESIGN
static const vector<Member> MEMBERS = {
    Member("", "MW9", "PROGRAMMING"),
    Member("", "W0N", "DESIGN"),
    Member("", "B3O", "ART"),
};

static const std::string FONT_NORMAL = "fonts/BorisBlackBloxx.ttf";

CreditPopup::CreditPopup() : BasePopup(Type::CREDIT) {
}

CreditPopup::~CreditPopup() {
}

bool CreditPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    initCredit();
    
    runEnterAction();
    
    return true;
}

/**
 * 등장 연출
 */
void CreditPopup::runEnterAction(SBCallback onFinished) {
    
    BasePopup::runEnterAction(onFinished);
    
    SBNodeUtils::recursiveCascadeOpacityEnabled(this, true);
    
    // fade in
    setOpacity(0);
    
    auto fadeIn = FadeIn::create(0.3f);
    auto callFunc = CallFunc::create([=]() {
        
        this->retain();
        
        if( onFinished ) {
            onFinished();
        }
        
        this->onEnterActionFinished();
        this->release();
    });
    runAction(Sequence::create(fadeIn, callFunc, nullptr));
}

/**
 * 등장 연출 완료
 */
void CreditPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
    
    // 화면 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode, SBZOrder::TOP);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->dismiss();
    });
}

void CreditPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
    
    setBackgroundColor(Color::POPUP_BG);
}

void CreditPopup::initCredit() {
    
    // Title
    auto title = Label::createWithTTF("3ERIES", FONT_RETRO, 80);
    title->setAnchorPoint(ANCHOR_MT);
    title->setPosition(Vec2TC(0, -100));
    addChild(title);
    
    // Member
    auto members = MEMBERS;
    random_shuffle(members.begin(), members.end());
    
    float posY = Vec2MC(0, 200).y;
    
    for( int i = 0; i < 3; ++i ) {
        auto member = members[i];
        
        auto job = Label::createWithTTF(member.job, FONT_NORMAL, 40,
                                        Size::ZERO,
                                        TextHAlignment::CENTER,
                                        TextVAlignment::TOP);
        job->setAnchorPoint(ANCHOR_MT);
        job->setPosition(Vec2(Vec2MC(0, 0).x, posY));
        addChild(job);
        
        posY -= job->getContentSize().height;
        posY -= 20;
        
        auto name = Label::createWithTTF(member.name, FONT_RETRO, 40,
                                         Size::ZERO,
                                         TextHAlignment::CENTER,
                                         TextVAlignment::TOP);
        name->setAnchorPoint(ANCHOR_MT);
        name->setPosition(Vec2(Vec2MC(0, 0).x, posY));
        addChild(name);
        
        posY -= name->getContentSize().height;
        posY -= 50;
    }
}

