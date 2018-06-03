//
//  PausePopup.cpp
//
//  Created by seongmin hwang on 2018. 2. 22..
//

#include "PausePopup.hpp"

#include "RSP.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

PausePopup::PausePopup() : SBBasePopup(),
contentsLayer(nullptr),
onClickMenuListener(nullptr) {
    
}

PausePopup::~PausePopup() {
    
}

bool PausePopup::init() {
    
    if( !SBBasePopup::init() ) {
        return false;
    }

    initBg();
    initTopMenu();
    initCenterMenu();
    
    return true;
}

void PausePopup::onExit() {
    
    SBBasePopup::onExit();
}

void PausePopup::initBg() {
    
    addChild(LayerColor::create(Color::POPUP_BG));
    
    contentsLayer = SBNodeUtils::createZeroSizeNode();
    addChild(contentsLayer);
}

void PausePopup::initTopMenu() {
    
    // mute
    /*
    auto muteBtn = SBToggleButton::create(DIR_IMG_COMMON + "common_sound_01.png",
                                          DIR_IMG_COMMON + "common_sound_02.png");
    muteBtn->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    muteBtn->setPosition(Vec2TR(-10, -10));
    contentsLayer->addChild(muteBtn);
    
    muteBtn->setSelected(SoundManager::getInstance()->isMute());
    muteBtn->setOnSelectedListener([=](bool isSelected) -> bool {
        
        bool isMute = isSelected;
        SoundManager::getInstance()->setMute(isMute);
        
        return true;
    });
    */
}

void PausePopup::initCenterMenu() {
 
    auto menuLayer = Node::create();
    contentsLayer->addChild(menuLayer);
    
    const Size MENU_SIZE(ButtonSize::LARGE);
    
    MenuType types[] = {
        MenuType::RESUME,
        MenuType::MAIN,
    };
    
    string titles[] = {
        "RESUME",
        "HOME",
    };
    
    float posY = 0;
    float paddingY = 50;
    
    auto setLabelEffect = [](Label *label) {
//        label->enableOutline(Color4B::BLACK, 3);
//        label->enableShadow(Color4B(221, 6, 6, 255), Size(1,-1));
    };
    
    for( int i = 0; i < sizeof(types)/sizeof(MenuType); ++i ) {
        MenuType type = types[i];
        
        auto btn = UIHelper::createFontButton(titles[i], MENU_SIZE);
        btn->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        btn->setPosition(Vec2(MENU_SIZE.width*0.5f, posY));
        menuLayer->addChild(btn);
        
        setLabelEffect(btn->getTitle());
        
        // 클릭 리스너
        btn->setOnClickListener([=](Node*) {
            
            if( onClickMenuListener ) {
                this->retain();
                onClickMenuListener(type);
                this->release();
            }
        });
        
        posY -= btn->getContentSize().height;
        posY -= paddingY;
    }
    
    Size size = SBNodeUtils::getChildrenBoundingSize(menuLayer);
    
    menuLayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    menuLayer->setPosition(Vec2MC(0, size.height));
    menuLayer->setContentSize(size);
//    menuLayer->setContentSize(SBNodeUtils::getChildrenBoundingSize(menuLayer));
//    menuLayer->setPosition(Vec2MC(0,
//                                  menuLayer->getContentSize().height*0.5f));
}


