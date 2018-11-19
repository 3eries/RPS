//
//  CharacterView.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#include "CharacterView.hpp"

#include <spine/spine-cocos2dx.h>

#include "Define.h"
#include "../game/GameDefine.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace spine;
using namespace std;

static const Color4B TEXT_COLOR_LOCKED              = Color4B(195,9,0,255);
static const Color4B TEXT_COLOR_UNLOCKED            = Color4B(69,0,202, 255);

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
 * 자체 업데이트
 */
void CharacterView::updateSelf() {
    
    setCharacter(character);
}

/**
 * 캐릭터 설정 
 */
void CharacterView::setCharacter(Character character) {
    
    this->character = character;
    
    const bool validCharacter = (character.charId != "");
    
    // visible 업데이트
    getChildByTag(Tag::COMING_SOON_VIEW)->setVisible(!validCharacter);
    
    Tag charViewTags[] = {
        Tag::UNLOCK_DESC,
        Tag::CHAR_NAME,
        Tag::CHAR_IMAGE,
    };
    
    for( auto tag : charViewTags ) {
        getChildByTag(tag)->setVisible(validCharacter);
    }
    
    for( auto label : unlockAmountLabels ) {
        label->setVisible(validCharacter);
    }
    
    // 캐릭터 업데이트
    if( !validCharacter ) {
        return;
    }
    
    const auto packDB = CharacterManager::getInstance()->getPackageDB(character.packId);
    const auto isUnlocked = packDB->isCharacterUnlocked(character.charId);
    
    // 잠금 해제 조건 설명
    getChildByTag<Label*>(Tag::UNLOCK_DESC)->setString(character.unlockDesc);
    
    // 잠금 해제 조건 진행도
    {
        int value = packDB->getUnlockFieldValue(character);
        int amount = character.unlockAmount;
        
        string format = "%" + STR_FORMAT("0%d", SBMath::getDigit(amount)) + "d";
        
        unlockAmountLabels[0]->setString(TO_STRING(value));
        // unlockAmountLabels[0]->setString(STR_FORMAT(format.c_str(), value));
        
        if( character.unlockAmountHidden && !isUnlocked ) {
            unlockAmountLabels[2]->setString("??");
        } else {
            unlockAmountLabels[2]->setString(TO_STRING(amount));
        }
        
        // 색깔 적용 및 좌표 가운데 정렬
        const auto textColor = isUnlocked ? TEXT_COLOR_UNLOCKED : TEXT_COLOR_LOCKED;
        
        float min = SB_BOUNDING_BOX_IN_WORLD(unlockAmountLabels[0]).getMinX();
        float max = SB_BOUNDING_BOX_IN_WORLD(unlockAmountLabels[2]).getMaxX();
        float width = max - min;
        float fixedX = (SB_WIN_SIZE.width - width) * 0.5f;
        float diff = fixedX - min;
        
        for( auto label : unlockAmountLabels ) {
            label->setTextColor(textColor);
            label->setPositionX(label->getPositionX() + diff);
        }
    }
    
    // 캐릭터 이름
    string nameFile = DIR_ADD(DIR_CHARACTER, character.packId) + character.charId + "_name.png";
    getChildByTag<Sprite*>(Tag::CHAR_NAME)->setTexture(nameFile);
    
    // 캐릭터 이미지
    auto anim = SBNodeUtils::createAnimation(character.idleAnims, HERO_ANIM_IDLE_DELAY_PER_UNIT);

//    auto charImage = getChildByTag<SBAnimationSprite*>(Tag::CHAR_IMAGE);
//    charImage->setAnimation(anim);
//    charImage->runAnimation();
//    runCharacterAnimation(true);
    /////////////////
    auto animate = Animate::create(anim);
    
    auto charImage = getChildByTag<EffectSprite*>(Tag::CHAR_IMAGE);
    charImage->stopAllActions();
    charImage->runAction(RepeatForever::create(animate));
    
    if( isUnlocked ) {
        charImage->setEffect(nullptr);
    } else {
        charImage->setEffect(Effect::create("shaders/example_GreyScale.fsh"));
    }
}

void CharacterView::runCharacterAnimation(bool isIdle) {
    
    auto anims = isIdle ? character.idleAnims : character.attackAnims;
    float delay = isIdle ? HERO_ANIM_IDLE_DELAY_PER_UNIT : HERO_ANIM_ATTACK_PER_UNIT;
    int loops = isIdle ? 3 : 2;
    
    auto anim = SBNodeUtils::createAnimation(anims, delay);
    
    auto charImage = getChildByTag<SBAnimationSprite*>(Tag::CHAR_IMAGE);
    charImage->setAnimation(anim, loops);
    charImage->runAnimation([=](Node*) {
        this->runCharacterAnimation(!isIdle);
    });
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
    
    auto comingSoonView = SBNodeUtils::createWinSizeNode();
    comingSoonView->setTag(Tag::COMING_SOON_VIEW);
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
    auto unlockDescLabel = Label::createWithTTF(character.unlockDesc, FONT_SABO, 36, Size::ZERO,
                                                TextHAlignment::CENTER, TextVAlignment::CENTER);
    unlockDescLabel->setTag(Tag::UNLOCK_DESC);
    unlockDescLabel->setAnchorPoint(ANCHOR_M);
    unlockDescLabel->setPosition(Vec2MC(0, 155));
    unlockDescLabel->setTextColor(Color4B(70,70,70,255));
    unlockDescLabel->enableShadow(Color4B(255,255,255,255), Size(0, -4));
    addChild(unlockDescLabel);
    
    // 잠금 해제 수량
    const int   FONT_SIZE = 64 - 4;
    const float POS_Y = 94;
    
    auto add = [=](Label *label) {

        // label->setTextColor(Color4B(195,9,0,255));
        label->enableShadow(Color4B(68,3,0,255), Size(0, -4));
        this->addChild(label);
        
        unlockAmountLabels.push_back(label);
    };
    
    auto label1 = Label::createWithTTF("0", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                       TextHAlignment::RIGHT, TextVAlignment::CENTER);
    label1->setAnchorPoint(ANCHOR_MR);
    label1->setPosition(Vec2MC(-10, POS_Y));
    add(label1);
    
    auto label2 = Label::createWithTTF("/", FONT_GAME_OVER, FONT_SIZE, Size::ZERO,
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    label2->setAnchorPoint(ANCHOR_M);
    label2->setPosition(Vec2MC(0, POS_Y));
    label2->setScale((label1->getContentSize().height / label2->getContentSize().height) * 1.3f);
    add(label2);
    
    auto label3 = Label::createWithTTF("0", FONT_RETRO, FONT_SIZE, Size::ZERO,
                                       TextHAlignment::LEFT, TextVAlignment::CENTER);
    label3->setAnchorPoint(ANCHOR_ML);
    label3->setPosition(Vec2MC(10, POS_Y));
    add(label3);
}

/**
 * 이미지 초기화
 */
void CharacterView::initImage() {
    
    // 캐릭터 이름
    auto charNameImage = Sprite::create();
    charNameImage->setTag(Tag::CHAR_NAME);
    charNameImage->setAnchorPoint(ANCHOR_M);
    charNameImage->setPosition(Vec2MC(0, -67));
    addChild(charNameImage);
    
    // 캐릭터 이미지
    // auto charImage = SBAnimationSprite::create();
    //
    auto charImage = EffectSprite::create();
    //
    charImage->setTag(Tag::CHAR_IMAGE);
    charImage->setAnchorPoint(ANCHOR_M);
    charImage->setPosition(Vec2MC(39, -224));
    addChild(charImage);
}


