//
//  RankingPopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 17..
//

#ifndef RankingPopup_hpp
#define RankingPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

#include "RankingManager.hpp"
#include "RecordRowView.hpp"

class RankingPopup : public BasePopup {
public:
    static const float SLIDE_IN_DURATION;
    static const float SLIDE_OUT_DURATION;
    static const float FADE_IN_DURATION;
    static const float FADE_OUT_DURATION;
    
    static const float CLOSE_BUTTON_SLIDE_DURATION;
    
public:
    CREATE_FUNC(RankingPopup);
    virtual ~RankingPopup();
    
protected:
    RankingPopup(Type type = Type::RANKING);
    
    virtual bool init() override;
    virtual void onEnter() override;
    
    virtual void initBackgroundView() override;
    virtual void initContentView() override;
    virtual void initRankings();
    
    virtual RankingRecords getRecords();
    
public:
    virtual void runEnterAction(SBCallback onFinished = nullptr) override;
    virtual void runExitAction(SBCallback onFinished = nullptr) override;
    
    void setCloseButtonEnabled(bool enabled);
    
protected:
    cocos2d::Node *stone;       // 배경 비석
    SBButton *closeBtn;         // 닫기 버튼
    
    cocos2d::Node *rankingView; // 랭킹 리스트
    std::vector<RecordRowView*> recordRowViews;
    
    std::vector<cocos2d::Node*> fadeNodes;
};

#endif /* RankingPopup_hpp */
