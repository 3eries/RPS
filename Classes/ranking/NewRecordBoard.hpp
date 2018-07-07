//
//  NewRecordBoard.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 5..
//

#ifndef NewRecordBoard_hpp
#define NewRecordBoard_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

class NewRecordBoard : public cocos2d::Node {
public:
    static const float EXIT_DURATION;
    
public:
    static NewRecordBoard* create(int score);
    ~NewRecordBoard();
    
private:
    NewRecordBoard(int score);
    
    bool init() override;
    
    void initBg();
    void initInitial();
    
    void setInitial(int nameIndex, int initialIndex);
    void initialUp(int nameIndex);
    void initialDown(int nameIndex);
    
public:
    std::string getName();
    
    void runEnterAction(float duration, SBCallback onFinished = nullptr);
    void runExitAction(SBCallback onFinished = nullptr);
    
    void runScoreAction();
    
private:
    int score;
    
    CC_SYNTHESIZE(std::function<void(std::string)>, onNameChangedListener,
                  OnNameChangedListener);
    CC_SYNTHESIZE(std::function<void(std::string)>, onInputCompletedListener,
                  OnInputCompletedListener);
    
    cocos2d::Label *scoreLabel;
    SBButton *enterBtn;
    
    struct NameInitial {
        cocos2d::Label *label;
        std::string str;
        int initialIndex;
    };
    
    std::vector<NameInitial> nameInitials;
};

#endif /* NewRecordBoard_hpp */
