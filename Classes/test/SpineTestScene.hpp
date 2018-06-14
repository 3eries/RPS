//
//  SpineTestScene.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#ifndef SpineTestScene_hpp
#define SpineTestScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseTestScene.hpp"

class SpineTestScene : public BaseTestScene, public cocos2d::ui::EditBoxDelegate {
public:
    CREATE_FUNC(SpineTestScene);
    ~SpineTestScene();
    
private:
    SpineTestScene();
    
    bool init() override;
    
    void initInputBox();
    void initAnimationStatusUI();
    
    void updateAnimation();
    
    void updateAnimationFileName(const std::string &str);
    void updateAnimationName(const std::string &str);
    void updateLoopForever(const std::string &str);
    void update(float dt) override;
    
    std::string findAnimationFile(const std::string &findFile);
    std::string findAnimationName(const std::string &findName);
    
private:
    virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)override;
    virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox)override;
    virtual void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, cocos2d::ui::EditBoxDelegate::EditBoxEndAction action)override;
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)override;
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox)override;
    
private:
    std::string animFileName;
    std::string animName;
    bool isLoopForever;
    
    cocos2d::ui::EditBox *animFileNameBox;
    cocos2d::ui::EditBox *animNameBox;
    cocos2d::ui::EditBox *loopForeverBox;
    
    spine::SkeletonAnimation *anim;
    
    cocos2d::Label *durationLabel;
    cocos2d::Label *statusLabel;
    
    cocos2d::Label *timeLabel;
    float elapsed;
    
    SBButton *runBtn;
    SBButton *stopBtn;
    SBButton *clearBtn;
};

#endif /* SpineTestScene_hpp */
