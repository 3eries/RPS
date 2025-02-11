//
//  RecordRowView.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 7..
//

#include "RecordRowView.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

const Size RecordRowView::SIZE          = Size(376, 34);
static const int FONT_SIZE              = 42;

static const Color3B NORMAL_COLOR       = Color3B(255, 255, 255);
 static const Color3B HIGHLIGHT_COLOR    = Color3B(255, 217, 0);
//static const Color3B HIGHLIGHT_COLOR    = Color3B(255, 255, 0);

RecordRowView* RecordRowView::create(RankingRecord record) {
    
    auto view = new RecordRowView();
    
    if( view && view->init() ) {
        view->setRecord(record);
        view->autorelease();
        return view;
    }
    
    delete view;
    return nullptr;
}

RecordRowView::RecordRowView() {
    
}

RecordRowView::~RecordRowView() {
    
}

bool RecordRowView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setContentSize(SIZE);
    
    // addChild(SBNodeUtils::createBackgroundNode(this, Color4B::BLACK), -1);
    
    // 순위
    rankingLabel = Label::createWithTTF("", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                        TextHAlignment::RIGHT, TextVAlignment::CENTER);
    rankingLabel->setAnchorPoint(ANCHOR_MR);
    rankingLabel->setPosition(Vec2ML(SIZE, 32, 0));
    addChild(rankingLabel);
    
    // .
    auto dot = Label::createWithTTF(".", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                    TextHAlignment::CENTER, TextVAlignment::CENTER);
    dot->setAnchorPoint(ANCHOR_M);
    dot->setPosition(Vec2ML(SIZE, 44, 0));
    addChild(dot);
    
    // no record
    /*
    if( record.name == "" ) {
        auto noRecord = Label::createWithTTF("NO RECORD", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                             TextHAlignment::LEFT, TextVAlignment::CENTER);
        noRecord->setColor(Color3B::WHITE);
        noRecord->setAnchorPoint(ANCHOR_ML);
        noRecord->setPosition(Vec2ML(size, 60 + 14, 0));
        addChild(noRecord);
        
        return layer;
    }
     */
    
    // 이름
    nameLabel = Label::createWithTTF("", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                     TextHAlignment::LEFT, TextVAlignment::CENTER);
    nameLabel->setAnchorPoint(ANCHOR_ML);
    nameLabel->setPosition(Vec2ML(SIZE, 60, 0));
    addChild(nameLabel);
    
    // -
    hyphenLabel = Label::createWithTTF("-", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    hyphenLabel->setAnchorPoint(ANCHOR_M);
    hyphenLabel->setPosition(Vec2MC(SIZE, 19, 0));
    addChild(hyphenLabel);
    
    // 점수
    scoreLabel = Label::createWithTTF("", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                      TextHAlignment::CENTER, TextVAlignment::CENTER);
    scoreLabel->setAnchorPoint(ANCHOR_M);
    scoreLabel->setPosition(Vec2MR(SIZE, -67, 0));
    addChild(scoreLabel);
    
    // new record
    newRecordLabel = Label::createWithTTF("NEW RECORD", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                          TextHAlignment::LEFT, TextVAlignment::CENTER);
    newRecordLabel->setAnchorPoint(ANCHOR_ML);
    newRecordLabel->setPosition(Vec2ML(SIZE, 60, 0));
    newRecordLabel->setVisible(false);
    addChild(newRecordLabel);
    
    SBNodeUtils::recursiveCascadeColorEnabled(this, true);
    SBNodeUtils::recursiveCascadeOpacityEnabled(this, true);
    
    changeToNormal();
    
    return true;
}

void RecordRowView::setRecord(RankingRecord record) {

    this->record = record;
    
    setRanking(record.ranking);
    setRecordName(record.name);
    setScore(record.score);
}

void RecordRowView::setRanking(int ranking) {
    
    record.ranking = ranking;
    
    rankingLabel->setString(STR_FORMAT("%d", ranking));
}

void RecordRowView::setRecordName(const string &name) {
 
    record.name = name;
    
    nameLabel->setString(record.name);
}

void RecordRowView::setScore(int score) {

    record.score = score;
    
    scoreLabel->setString(TO_STRING(score));
}

/**
 * new record 설정
 */
void RecordRowView::setNewRecord(bool enabled) {
    
    Node *normalNodes[] = { nameLabel, scoreLabel, hyphenLabel, };
    
    for( auto n : normalNodes ) {
        n->setVisible(!enabled);
    }
    
    newRecordLabel->setVisible(enabled);
}

/**
 * normal 상태로 변경
 */
void RecordRowView::changeToNormal() {
    
    changeNameToNormal();
    
    setColor(NORMAL_COLOR);
    
    stopAllActions();
    setOpacity(255);
}

/**
 * 하이라이트 상태로 변경
 */
void RecordRowView::changeToHighlight() {
    
    changeToNormal();
    
    setColor(HIGHLIGHT_COLOR);
    
    // 연출
    auto fadeOut = FadeOut::create(0.5f);
    auto callFuncN = CallFuncN::create([=](Node *sender) {
        
//        auto fadeIn = FadeIn::create(0.7f);
//        auto fadeOut = FadeOut::create(0.1f);
        auto fadeIn = FadeIn::create(0.7f);
        auto fadeOut = FadeOut::create(0.2f);
        auto delay = DelayTime::create(0.3f);
        
        auto seq = Sequence::create(fadeIn, delay, fadeOut, nullptr);
        sender->runAction(RepeatForever::create(seq));
    });
    
    runAction(Sequence::create(fadeOut, callFuncN, nullptr));
}

/**
 * 이름 normal 상태로 변경
 */
void RecordRowView::changeNameToNormal() {
    
    setColor(NORMAL_COLOR);
    
    nameLabel->stopAllActions();
    nameLabel->setOpacity(255);
}

/**
 * 이름 하이라이트 상태로 변경
 */
void RecordRowView::changeNameToHighlight() {
    
    changeNameToNormal();
    
    setColor(HIGHLIGHT_COLOR);
    
    // 연출
    auto fadeOut = FadeOut::create(0.5f);
    auto callFuncN = CallFuncN::create([=](Node *sender) {
        
        //        auto fadeIn = FadeIn::create(0.7f);
        //        auto fadeOut = FadeOut::create(0.1f);
        auto fadeIn = FadeIn::create(0.7f);
        auto fadeOut = FadeOut::create(0.2f);
        auto delay = DelayTime::create(0.3f);
        
        auto seq = Sequence::create(fadeIn, delay, fadeOut, nullptr);
        sender->runAction(RepeatForever::create(seq));
    });
    
    nameLabel->runAction(Sequence::create(fadeOut, callFuncN, nullptr));
}

