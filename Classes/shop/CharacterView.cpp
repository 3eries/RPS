//
//  CharacterView.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#include "CharacterView.hpp"

#include <spine/spine-cocos2dx.h>

#include "RSP.h"
#include "../game/GameDefine.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

CharacterView* CharacterView::create() {
    
    auto characterView = new CharacterView();
    
    if( characterView && characterView->init() ) {
        characterView->autorelease();
        return characterView;
    }
    
    delete characterView;
    return nullptr;
}

CharacterView::CharacterView() {
}

CharacterView::~CharacterView() {
    
}

bool CharacterView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    initBg();
    initComingSoon();
    initLabel();
    initImage();
    
    return true;
}

void CharacterView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

/**
 * 캐릭터 설정 
 */
void CharacterView::setCharacter(Character character) {
    
    this->character = character;
    
    const bool validCharacter = (character.charId != "");
    
    // visible 업데이트
    comingSoonView->setVisible(!validCharacter);
    
    Node *charViews[] = {
        unlockDescLabel,
        characterTitleImage,
        characterImage,
    };
    
    for( auto v : charViews ) {
        v->setVisible(validCharacter);
    }
    
    for( auto label : unlockAmountLabels ) {
        label->setVisible(validCharacter);
    }
    
    // 캐릭터 업데이트
    if( validCharacter ) {
        auto packDB = CharacterManager::getInstance()->getPackageDB(character.packId);
        
        unlockDescLabel->setString(character.unlockDesc);
        
        unlockAmountLabels[0]->setString(TO_STRING(packDB->getUnlockFieldValue(character)));
        unlockAmountLabels[2]->setString(TO_STRING(character.unlockAmount));
        
        string titleFile = DIR_ADD(DIR_CHARACTER, character.packId) + character.charId + "_name.png";
        characterTitleImage->setTexture(titleFile);
        
        auto anim = SBNodeUtils::createAnimation(character.idleAnims, HERO_ANIM_IDLE_DELAY_PER_UNIT);
        characterImage->setAnimation(anim);
        characterImage->runAnimation();
    }
}

Node* CharacterView::getBackground() {
    
    return getChildByTag(Tag::BG);
}

void CharacterView::initBg() {
    
    // RSP_popup_bg_shop.png Vec2MC(0, -102) , Size(552, 660)
    auto bg = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_shop.png");
    bg->setTag(Tag::BG);
    bg->setAnchorPoint(ANCHOR_M);
    bg->setPosition(Vec2MC(0, -102));
    addChild(bg);
}

/**
 * 커밍순 뷰 초기화
 */
void CharacterView::initComingSoon() {
    
    comingSoonView = SBNodeUtils::createWinSizeNode();
    comingSoonView->setVisible(false);
    addChild(comingSoonView);
    
    // RSP_popup_bg_shop_question_mark.png Vec2MC(0, -82) , Size(288, 460)
    auto mark = Sprite::create(DIR_IMG_GAME + "RSP_popup_bg_shop_question_mark.png");
    mark->setAnchorPoint(ANCHOR_M);
    mark->setPosition(Vec2MC(0, -82));
    comingSoonView->addChild(mark);
}

/**
 * 라벨 초기화
 */
void CharacterView::initLabel() {
    
    // 잠금 해제 조건 설명
    // LAUNCH THE GAME size:36 color:70,70,70 shadow:255,255,255 Vec2MC(-2, 155) , Size(367, 28)
    unlockDescLabel = Label::createWithTTF(character.unlockDesc, FONT_SABO, 36, Size::ZERO,
                                           TextHAlignment::CENTER, TextVAlignment::CENTER);
    unlockDescLabel->setAnchorPoint(ANCHOR_M);
    unlockDescLabel->setPosition(Vec2MC(0, 155));
    unlockDescLabel->setTextColor(Color4B(70,70,70,255));
    addChild(unlockDescLabel);
    
    // 잠금 해제 조건 수량
    // 50/70 size:74 color:195,9,0 shadow:68,3,0 Vec2MC(-1, 94) , Size(227, 64)
    // 보라색 : 69,0,202
    auto add = [=](Label *label) {

        label->setTextColor(Color4B(195,9,0,255));
        label->enableShadow(Color4B(68,3,0,255), Size(0, -4));
        this->addChild(label);
        
        unlockAmountLabels.push_back(label);
    };
    
    auto label1 = Label::createWithTTF("0", FONT_RETRO, 64,
                                       Size::ZERO,
                                       TextHAlignment::RIGHT, TextVAlignment::CENTER);
    label1->setAnchorPoint(ANCHOR_MR);
    label1->setPosition(Vec2MC(-10, 94));
    add(label1);
    
    auto label2 = Label::createWithTTF("/", FONT_GAME_OVER, 64,
                                       Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
    label2->setAnchorPoint(ANCHOR_M);
    label2->setPosition(Vec2MC(0, 94));
    label2->setScale((label1->getContentSize().height / label2->getContentSize().height) * 1.3f);
    add(label2);
    
    auto label3 = Label::createWithTTF("0", FONT_RETRO, 64,
                                       Size::ZERO,
                                       TextHAlignment::LEFT, TextVAlignment::CENTER);
    label3->setAnchorPoint(ANCHOR_ML);
    label3->setPosition(Vec2MC(10, 94));
    add(label3);
}

/**
 * 이미지 초기화
 */
void CharacterView::initImage() {
    
    // 타이틀
    // RSP_popup_text_zeus.png Vec2MC(0, -67) , Size(486, 218)
    characterTitleImage = Sprite::create();
    characterTitleImage->setAnchorPoint(ANCHOR_M);
    characterTitleImage->setPosition(Vec2MC(0, -67));
    addChild(characterTitleImage);
    
    // 캐릭터
    // RSP_zeus_idle1.png Vec2MC(39, -224) , Size(388, 296)
    characterImage = SBAnimationSprite::create();
    characterImage->setAnchorPoint(ANCHOR_M);
    characterImage->setPosition(Vec2MC(39, -224));
    addChild(characterImage);
}


