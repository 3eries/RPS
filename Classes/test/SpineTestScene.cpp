//
//  SpineTestScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "SpineTestScene.hpp"

#include <spine/spine-cocos2dx.h>

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace spine;
using namespace cocos2d::ui;
using namespace std;

static const string ANIM_FILES[] = {
    ANIM_LOGO, ANIM_CLOUD,
    ANIM_FEVER_MODE_BG, ANIM_FEVER_MODE_FIRE,
    ANIM_DRAW, ANIM_DIE,
};

static const string ANIM_NAMES[] = {
    ANIM_NAME_RUN, ANIM_NAME_CLEAR,
//    ANIM_NAME_DRAW_LEFT, ANIM_NAME_DRAW_RIGHT,
    ANIM_NAME_DIE_LEFT, ANIM_NAME_DIE_RIGHT,
};

SpineTestScene::SpineTestScene() :
anim(nullptr) {
    
}

SpineTestScene::~SpineTestScene() {
    
}

bool SpineTestScene::init() {
    
    if( !BaseTestScene::init() ) {
        return false;
    }
    
    initInputBox();
    initAnimationStatusUI();
    
    updateAnimationFileName(animFileNameBox->getText());
    updateAnimationName(animNameBox->getText());
    updateLoopForever(loopForeverBox->getText());
    
    return true;
}

/**
 * 입력 박스 초기화
 */
void SpineTestScene::initInputBox() {
    
    auto editBoxSize = Size(SB_WIN_SIZE.width*0.75f, 50);
    auto fontSize = 30;
    
    auto createEditBox = [=](string title, string text, float posY) -> EditBox* {
        
        auto editBox = EditBox::create(editBoxSize, DIR_IMG_COMMON + "orange_edit.png");
        editBox->setText(text.c_str());
        editBox->setAnchorPoint(ANCHOR_TR);
        editBox->setPosition(Vec2TR(0, posY));
        editBox->setFontColor(Color3B::BLACK);
        editBox->setFontSize(fontSize);
        editBox->setDelegate(this);
        this->addChild(editBox);
        
        auto titleLabel = Label::createWithTTF(title, FONT_RETRO, fontSize,
                                               Size(0, editBoxSize.height),
                                               TextHAlignment::CENTER, TextVAlignment::CENTER);
        titleLabel->setColor(Color3B::WHITE);
        titleLabel->setAnchorPoint(ANCHOR_TL);
        titleLabel->setPosition(Vec2TL(0, posY));
        titleLabel->enableOutline(Color4B::BLACK, 4);
        addChild(titleLabel);
        
        return editBox;
    };
    
    animFileNameBox = createEditBox("file: ", ANIM_LOGO, -10);
//    animFileNameBox->setPlaceHolder("Password:");
//    animFileNameBox->setMaxLength(6);
//    animFileNameBox->setInputFlag(ui::EditBox::InputFlag::);
    animFileNameBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    
    animNameBox = createEditBox("anim: ", ANIM_NAME_RUN, -10 - editBoxSize.height * 1);
    animNameBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    
    loopForeverBox = createEditBox("loop: ", "0", -10 - editBoxSize.height * 2);
    loopForeverBox->setMaxLength(1);
    loopForeverBox->setInputMode(ui::EditBox::InputMode::DECIMAL);
    
    // update button
    auto updateBtn = UIHelper::createFontButton("UPDATE", ButtonSize::MEDIUM);
    updateBtn->setAnchorPoint(ANCHOR_MT);
    updateBtn->setPosition(Vec2TC(0, -170));
    addChild(updateBtn);
    
    updateBtn->addChild(SBNodeUtils::createBackgroundNode(updateBtn, Color4B::BLACK), -1);
    
    updateBtn->setOnClickListener([=](Node*) {
        
        CCLOG("findAnimationFile(animFileName): [%s], %s", animFileName.c_str(), findAnimationFile(animFileName).c_str());
        CCLOG("findAnimationName(animName): [%s], %s", animName.c_str(), findAnimationName(animName).c_str());
        
        // find file
        if( findAnimationFile(animFileName) == "" ) {
            MessageBox("not found file.", "");
            return;
        }
        
        // find anim
        if( findAnimationName(animName) == "" ) {
            MessageBox("not found animation.", "");
            return;
        }
        
        // init
        this->updateAnimation();
    });
}

/**
 * 애니메이션 상태 표시 UI 초기화
 */
void SpineTestScene::initAnimationStatusUI() {
    
    // duration
    durationLabel = Label::createWithTTF("", FONT_RETRO, 35,
                                         Size(SB_WIN_SIZE.width*0.95f, 0),
                                         TextHAlignment::CENTER, TextVAlignment::TOP);
    durationLabel->setAnchorPoint(ANCHOR_MT);
    durationLabel->setPosition(Vec2TC(0, -300));
    durationLabel->setColor(Color3B::WHITE);
    durationLabel->enableOutline(Color4B::BLACK, 4);
    addChild(durationLabel);
    
    // status
    timeLabel = Label::createWithTTF("", FONT_RETRO, 35,
                                     Size(SB_WIN_SIZE.width*0.95f, 0),
                                     TextHAlignment::CENTER, TextVAlignment::TOP);
    timeLabel->setAnchorPoint(ANCHOR_MT);
    timeLabel->setPosition(Vec2TC(0, -350));
    timeLabel->setColor(Color3B::WHITE);
    timeLabel->enableOutline(Color4B::BLACK, 4);
    addChild(timeLabel);
    
    // status
    statusLabel = Label::createWithTTF("", FONT_RETRO, 40,
                                       Size(SB_WIN_SIZE.width*0.95f, 0),
                                       TextHAlignment::CENTER, TextVAlignment::TOP);
    statusLabel->setAnchorPoint(ANCHOR_MT);
    statusLabel->setPosition(Vec2TC(0, -400));
    statusLabel->setColor(Color3B::WHITE);
    statusLabel->enableOutline(Color4B::BLACK, 4);
    addChild(statusLabel);
    
    // run button
    runBtn = UIHelper::createFontButton("RUN", ButtonSize::MEDIUM);
    runBtn->setAnchorPoint(ANCHOR_BR);
    runBtn->setPosition(Vec2BC(-130, 150));
    addChild(runBtn);
    
    // stop button
    stopBtn = UIHelper::createFontButton("STOP", ButtonSize::MEDIUM);
    stopBtn->setAnchorPoint(ANCHOR_MB);
    stopBtn->setPosition(Vec2BC(0, 150));
    addChild(stopBtn);
    
    // clear button
    clearBtn = UIHelper::createFontButton("CLEAR", ButtonSize::MEDIUM);
    clearBtn->setAnchorPoint(ANCHOR_BL);
    clearBtn->setPosition(Vec2BC(130, 150));
    addChild(clearBtn);
}

/**
 * 애니메이션 업데이트
 */
void SpineTestScene::updateAnimation() {
    
    string animFileName = findAnimationFile(this->animFileName);
    string animName = findAnimationName(this->animName);
    
    if( anim ) {
        anim->removeFromParent();
    }
    
    anim = SkeletonAnimation::createWithJsonFile(animFileName);
    anim->setAnchorPoint(Vec2::ZERO);
    anim->setPosition(Vec2MC(0,0));
    addChild(anim, -1);
    
    // duration
    float duration = anim->getAnimationDuration(animName);
    durationLabel->setString(STR_FORMAT("duration : %f", duration));
    
    // run button
    runBtn->setOnClickListener([=](Node*) {
        anim->clearTracks();
        auto track = anim->setAnimation(0, animName, isLoopForever);
        
        // listener
        {
            auto log = [=](string str) {
                statusLabel->setString(str);
                CCLOG("%s", str.c_str());
            };
            
            anim->setTrackStartListener(track, [=](spTrackEntry *track) {
                log(STR_FORMAT("%s -> start", track->animation->name));
            });
            
            anim->setTrackInterruptListener(track, [=](spTrackEntry *track) {
                log(STR_FORMAT("%s -> interrupt", track->animation->name));
            });
            
            anim->setTrackEndListener(track, [=](spTrackEntry *track) {
                log(STR_FORMAT("%s -> end loop: %d", track->animation->name, track->loop));
            });
            
            anim->setTrackDisposeListener(track, [=](spTrackEntry *track) {
                log(STR_FORMAT("%s -> dispose", track->animation->name));
            });
            
            anim->setTrackCompleteListener(track, [=](spTrackEntry *track) {
                log(STR_FORMAT("%s -> complete", track->animation->name));
                this->unscheduleUpdate();
            });
            
            anim->setTrackEventListener(track, [=](spTrackEntry *track, spEvent *event) {
                log(STR_FORMAT("%s -> event : %s", track->animation->name, event->data->name));
            });
        }
        
        elapsed = 0;
        timeLabel->setString("");
        
        this->scheduleUpdate();
    });
    
    // stop button
    stopBtn->setOnClickListener([=](Node*) {
        anim->clearTracks();
        this->unscheduleUpdate();
    });
    
    // clear button
    clearBtn->setOnClickListener([=](Node*) {
        SBSpineHelper::clearAnimation(anim, ANIM_NAME_CLEAR);
    });
}

void SpineTestScene::updateAnimationFileName(const string &str) {
    
    animFileName = findAnimationFile(str);
    
    // 파일명만
    size_t pos = animFileName.find_last_of("/");
    if( pos != string::npos ) {
        animFileName = animFileName.substr(pos+1);
    }
    
    animFileNameBox->setText(animFileName.c_str());
}

void SpineTestScene::updateAnimationName(const string &str) {
    
    animName = findAnimationName(str);
    animNameBox->setText(animName.c_str());
}

void SpineTestScene::updateLoopForever(const string &str) {
    
    isLoopForever = (str == "1");
    loopForeverBox->setText(STR_FORMAT("%d", isLoopForever).c_str());
}

void SpineTestScene::update(float dt) {
    
    elapsed += dt;
    timeLabel->setString(STR_FORMAT("elapsed : %f", elapsed));
}

string SpineTestScene::findAnimationFile(const string &findFile) {
    
    if( findFile == "" ) {
        return "";
    }
    
    for( auto file : ANIM_FILES ) {
        if( SBStringUtils::isContains(file, findFile) ) {
            return file;
        }
    }
    
    return "";
}

string SpineTestScene::findAnimationName(const string &findName) {

    if( findName == "" ) {
        return "";
    }
    
    for( auto name : ANIM_NAMES ) {
        if( SBStringUtils::isContains(name, findName) ) {
            return name;
        }
    }
    
    return "";
}

void SpineTestScene::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
//    CCLOG("editBox %p DidBegin !", editBox);
}

void SpineTestScene::editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox)
{
//    CCLOG("editBox %p DidEnd !", editBox);
}

void SpineTestScene::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, cocos2d::ui::EditBoxDelegate::EditBoxEndAction action)
{
//    CCLOG("editBox %p DidEnd with action %d!", editBox, action);
}

void SpineTestScene::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
//    CCLOG("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void SpineTestScene::editBoxReturn(ui::EditBox* editBox) {
    
//    CCLOG("editBox %p was returned !",editBox);
    
    if( animFileNameBox == editBox ) {
        updateAnimationFileName(animFileNameBox->getText());
    }
    else if( animNameBox == editBox ) {
        updateAnimationName(animNameBox->getText());
    }
    else if( loopForeverBox == editBox ) {
        updateLoopForever(loopForeverBox->getText());
    }
}

