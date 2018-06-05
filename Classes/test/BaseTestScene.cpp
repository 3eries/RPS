//
//  BaseTestScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "BaseTestScene.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

BaseTestScene::BaseTestScene() {
    
}

BaseTestScene::~BaseTestScene() {
    
}

bool BaseTestScene::init() {
    
    if( !Scene::init() ) {
        return false;
    }
    
    // back key
    {
        auto listener = EventListenerKeyboard::create();
        listener->onKeyReleased = [=] (EventKeyboard::KeyCode keyCode, Event *event) {
            
            if( keyCode != EventKeyboard::KeyCode::KEY_BACK ) {
                return;
            }
            
            this->onBack();
        };
        
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
    
    auto backBtn = UIHelper::createFontButton("BACK", ButtonSize::MEDIUM);
    backBtn->setAnchorPoint(ANCHOR_MB);
    backBtn->setPosition(Vec2BC(0, 10));
    addChild(backBtn);
    
    backBtn->setOnClickListener([=](Node*) {
        this->onBack();
    });
    
    return true;
}

void BaseTestScene::onEnter() {
    
    Scene::onEnter();
}

void BaseTestScene::onEnterTransitionDidFinish() {
    
    Scene::onEnterTransitionDidFinish();
}

void BaseTestScene::onExit() {
    
    Scene::onExit();
}

void BaseTestScene::onBack() {
    
    Director::getInstance()->popScene();
}
