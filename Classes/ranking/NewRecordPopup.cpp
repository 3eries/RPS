//
//  NewRecordPopup.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 7..
//

#include "NewRecordPopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"
#include "SceneManager.h"
#include "UserDefaultKey.h"

#include "NewRecordBoard.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const int   VISIBLE_RANKING_COUNT = 6;
static const float DISMISS_DELAY         = 2.0f;

#define RECORD_BOARD_POS                 Vec2BC(0, 288)

NewRecordPopup* NewRecordPopup::create(int ranking, int score) {
    
    auto popup = new NewRecordPopup(ranking, score);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    delete popup;
    return nullptr;
}

NewRecordPopup::NewRecordPopup(int ranking, int score) : RankingPopup(Type::NEW_RECORD),
onRecordCompletedListener(nullptr) {
    
    record.ranking = ranking;
    record.score = score;
}

NewRecordPopup::~NewRecordPopup() {
    
}

bool NewRecordPopup::init() {
    
    if( !RankingPopup::init() ) {
        return false;
    }
    
    CCLOG("NewRecordPopup ranking: %d score: %d", record.ranking, record.score);
    
    return true;
}

void NewRecordPopup::onEnter() {
    
    RankingPopup::onEnter();
    
    runEnterAction();
}

void NewRecordPopup::initBackgroundView() {
    
    RankingPopup::initBackgroundView();
}

void NewRecordPopup::initContentView() {
    
    RankingPopup::initContentView();
}

void NewRecordPopup::initRankings() {
    
    RankingPopup::initRankings();
    
    // 입력창
    // RSP_popup_bg_new_record.png Vec2BC(0, 288) , Size(696, 536)
    recordBoard = NewRecordBoard::create(record.score);
    recordBoard->setAnchorPoint(ANCHOR_M);
    recordBoard->setPosition(RECORD_BOARD_POS);
    recordBoard->setVisible(false);
    addChild(recordBoard, SBZOrder::BOTTOM);
    
    // 이름 변경
    recordBoard->setOnNameChangedListener([=](string name) {
        
        record.name = name;
        getRecordRow()->setRecordName(name);
    });
    
    // 입력 완료
    recordBoard->setOnInputCompletedListener([=](string name) {
        
        record.name = name;
        this->onRecordCompleted();
    });
}

/**
 * 기록 완료
 */
void NewRecordPopup::onRecordCompleted() {
    
    onRecordCompletedListener(record);
    
    // 이름 저장
    UserDefault::getInstance()->setStringForKey(UserDefaultKey::LAST_RECORD_NAME, record.name);
    UserDefault::getInstance()->flush();
    
    // row 하이라이트
    getRecordRow()->changeToHighlight();
    
    // 보드 퇴장 연출
    recordBoard->runExitAction([=]() {
        recordBoard->setVisible(false);
    });
    
    // 랭킹 리스트 좌표 복원 연출
    if( record.ranking > VISIBLE_RANKING_COUNT ) {
        rankingView->runAction(MoveTo::create(NewRecordBoard::EXIT_DURATION, Vec2(0,0)));
    }
    
    // n초 후 팝업 종료
    auto delay = DelayTime::create(DISMISS_DELAY);
    auto callFunc = CallFunc::create([=]() {
        this->dismissWithAction();
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

RecordRowView* NewRecordPopup::getRecordRow() {
    return recordRowViews[record.ranking-1];
}

RankingRecords NewRecordPopup::getRecords() {
    
    auto records = RankingManager::getTopRecords(10);
    
    // 신기록 추가
    int i = record.ranking-1;
    records.insert((records.begin() + i), record);
    
    // 밀린 기록 삭제
    records.erase((records.begin() + records.size()-1));
    
    RankingManager::sort(records);
    
    return records;
}

/**
 * 등장 연출
 */
void NewRecordPopup::runEnterAction(SBCallback onFinished) {
    
    RankingPopup::runEnterAction(onFinished);
    
    // 신기록 달성 연출
    auto row = getRecordRow();
    // row->changeNameToHighlight();
    row->setNewRecord(true);
    row->changeToHighlight();
    
    // idle 상태 후 입력창 등장
    auto delay = DelayTime::create(2.5f);
    auto callFunc = CallFunc::create([=]() {
        
        // 신기록 Row 연출 변경
        row->setNewRecord(false);
        row->changeToNormal();
        row->changeNameToHighlight();
        row->setRecordName(recordBoard->getName());
        
        // 랭킹이 입력창에 가려질 경우, 랭킹 리스트 좌표 조정
        const float DURATION = RankingPopup::SLIDE_IN_DURATION;
        
        if( record.ranking > VISIBLE_RANKING_COUNT ) {
            // rankingView->setPositionY(240);
            rankingView->runAction(MoveTo::create(DURATION, Vec2(0, 240)));
        }
        
        // 보드 연출
        recordBoard->setVisible(true);
        recordBoard->runEnterAction(DURATION);
    });
    runAction(Sequence::create(delay, callFunc, nullptr));
}

/**
 * 퇴장 연출
 */
void NewRecordPopup::runExitAction(SBCallback onFinished) {
    
    RankingPopup::runExitAction(onFinished);
    // RankingPopup::runExitAction(SLIDE_OUT_DURATION * 0.7f, onFinished);
}

