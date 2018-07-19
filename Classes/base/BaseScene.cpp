//
//  BaseScene.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 19..
//

#include "BaseScene.hpp"

#include "RSP.h"

USING_NS_CC;
using namespace std;

BaseScene::BaseScene() :
commonMenu(nullptr) {
    
}

BaseScene::~BaseScene() {
    
}

void BaseScene::initCommonMenu() {
    
    commonMenu = CommonMenu::create();
    commonMenu->setTopMenu(this, PopupZOrder::TOP);
    commonMenu->setBottomMenu(this, PopupZOrder::MIDDLE);
    addChild(commonMenu);
}

