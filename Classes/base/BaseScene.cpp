//
//  BaseScene.cpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 19..
//

#include "BaseScene.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

BaseScene::BaseScene() :
commonMenu(nullptr) {
    
}

BaseScene::~BaseScene() {
    
}

bool BaseScene::init() {
    
    if( !Scene::init() ) {
        return false;
    }
    
    processBackKey();
    
    return true;
}

void BaseScene::initCommonMenu() {
    
    commonMenu = CommonMenu::create();
    commonMenu->setTopMenu(this, PopupZOrder::TOP);
    commonMenu->setBottomMenu(this, PopupZOrder::MIDDLE);
    addChild(commonMenu);
}

