//
//  RecordRowView.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 7..
//

#ifndef RecordRowView_hpp
#define RecordRowView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "RankingManager.hpp"

class RecordRowView : public cocos2d::Node {
public:
    static const cocos2d::Size SIZE;
    
public:
    static RecordRowView* create(RankingRecord record);
    ~RecordRowView();
    
private:
    RecordRowView();
    
    bool init() override;
    
public:
    void setRecord(RankingRecord record);
    void setRanking(int ranking);
    void setRecordName(const std::string &name);
    void setScore(int score);
    
    void setNewRecord(bool enabled);
    
    void changeToNormal();
    void changeToHighlight();
    
    void changeNameToNormal();
    void changeNameToHighlight();
    
private:
    CC_SYNTHESIZE_READONLY(RankingRecord, record, Record);
    
    cocos2d::Label *rankingLabel;
    cocos2d::Label *nameLabel;
    cocos2d::Label *scoreLabel;
    cocos2d::Label *hyphenLabel;
    cocos2d::Label *newRecordLabel;
};

#endif /* RecordRowView_hpp */
