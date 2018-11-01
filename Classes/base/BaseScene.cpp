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
    
    // Key 이벤트 리스너 등록
    auto listener = EventListenerKeyboard::create();
    listener->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event *event) {
        
        if( keyCode == EventKeyboard::KeyCode::KEY_BACK ) {
            this->onBackKeyReleased();
        }
    };
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void BaseScene::initCommonMenu() {
    
    commonMenu = CommonMenu::create();
    commonMenu->setTopMenu(this, ZOrder::POPUP_TOP);
    commonMenu->setBottomMenu(this, ZOrder::POPUP_MIDDLE);
    addChild(commonMenu);
}

