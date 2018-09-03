//
//  NewRecordPopup.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 7..
//

#ifndef NewRecordPopup_hpp
#define NewRecordPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RankingPopup.hpp"

class NewRecordBoard;

class NewRecordPopup : public RankingPopup {
public:
    static NewRecordPopup* create(int ranking, int score);
    virtual ~NewRecordPopup();
    
protected:
    NewRecordPopup(int ranking, int score);
    
    virtual bool init() override;
    virtual void onEnter() override;
    
    virtual void initBackgroundView() override;
    virtual void initContentView() override;
    void initRankings() override;
    
    void onRecordCompleted();
    
    RecordRowView* getRecordRow();
    RankingRecords getRecords() override;
    
public:
    virtual void runEnterAction(SBCallback onFinished = nullptr) override;
    virtual void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
protected:
    CC_SYNTHESIZE(std::function<void(RankingRecord)>, onRecordCompletedListener,
                  OnRecordCompletedListener);
    
    RankingRecord record;
    NewRecordBoard *recordBoard;
};

#endif /* NewRecordPopup_hpp */
