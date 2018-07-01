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

static const std::string FONT_NORMAL = "fonts/BorisBlackBloxx.ttf";

void CreditPopup::show(Node *parent, int zOrder) {
    
    CCASSERT(parent != nullptr, "CreditPopup::show error: invalid parent.");
    
    auto layer = SBNodeUtils::createWinSizeNode();
    
    auto popup = CreditPopup::create();
    parent->addChild(popup, zOrder);
    
    //
}

CreditPopup::CreditPopup() : SBBasePopup(),
onClosedListener(nullptr),
contentView(nullptr) {
    
}

CreditPopup::~CreditPopup() {
    
}

bool CreditPopup::init() {
    
    if( !SBBasePopup::init() ) {
        return false;
    }
    
    initBg();
    initCredit();
    
    return true;
}

void CreditPopup::onExit() {
    
    SBBasePopup::onExit();
}

void CreditPopup::close() {
    
    retain();
    
    if( onClosedListener ) {
        onClosedListener();
    }
    
    removeFromParent();
    release();
}

/**
 * 화면 연출 완료
 */
void CreditPopup::onActionCompleted() {
    
    // 화면 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode, SBZOrder::TOP);
    
    touchNode->addClickEventListener([=](Ref*) {
        this->close();
    });
}

void CreditPopup::initBg() {
    
    auto bg = LayerColor::create(Color::POPUP_BG);
    addChild(bg);
    
    contentView = SBNodeUtils::createZeroSizeNode();
    addChild(contentView);
    
    SBNodeUtils::recursiveCascadeOpacityEnabled(contentView, true);
    
    // 배경 연출
    bg->setOpacity(0);
    
    const float DURATION = 0.3f;
    bg->runAction(FadeTo::create(DURATION, Color::POPUP_BG.a));
    
    // 컨텐츠 연출
    contentView->setOpacity(0);
    
    auto fadeIn = FadeIn::create(DURATION);
    auto callFunc = CallFunc::create([=]() {
        this->onActionCompleted();
    });
    contentView->runAction(Sequence::create(fadeIn, callFunc, nullptr));
}

void CreditPopup::initCredit() {
    
    // Title
    auto title = Label::createWithTTF("3ERIES", FONT_RETRO, 80);
    title->setAnchorPoint(ANCHOR_MT);
    title->setPosition(Vec2TC(0, -100));
    contentView->addChild(title);
    
    // Member
    struct Member {
        string image;
        string name;
        string job;
        
        Member(string _image, string _name, string _job) :
        image(_image), name(_name), job(_job) {}
    };
    
    // ART PROGRAMMING DESIGN
    float posY = Vec2MC(0, 200).y;
    
    vector<Member> members = {
        Member("", "MW9", "PROGRAMMING"),
        Member("", "W0N", "DESIGN"),
        Member("", "B3O", "ART"),
    };
    
    random_shuffle(members.begin(), members.end());
    
    for( int i = 0; i < 3; ++i ) {
        auto member = members[i];
        
        auto job = Label::createWithTTF(member.job, FONT_NORMAL, 40,
                                        Size::ZERO,
                                        TextHAlignment::CENTER,
                                        TextVAlignment::TOP);
        job->setAnchorPoint(ANCHOR_MT);
        job->setPosition(Vec2(Vec2MC(0, 0).x, posY));
        contentView->addChild(job);
        
        posY -= job->getContentSize().height;
        posY -= 20;
        
        auto name = Label::createWithTTF(member.name, FONT_RETRO, 40,
                                         Size::ZERO,
                                         TextHAlignment::CENTER,
                                         TextVAlignment::TOP);
        name->setAnchorPoint(ANCHOR_MT);
        name->setPosition(Vec2(Vec2MC(0, 0).x, posY));
        contentView->addChild(name);
        
        posY -= name->getContentSize().height;
        posY -= 50;
    }
}

