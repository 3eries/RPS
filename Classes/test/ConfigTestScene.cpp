//
//  ConfigTestScene.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 23..
//

#include "ConfigTestScene.hpp"

#include "RSP.h"
#include "SceneManager.h"
#include "UIHelper.hpp"

#include "PatchManager.hpp"
#include "GameConfiguration.hpp"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

ConfigTestScene::ConfigTestScene() {
    
}

ConfigTestScene::~ConfigTestScene() {
    
}

bool ConfigTestScene::init() {
    
    if( !BaseTestScene::init() ) {
        return false;
    }
    
    // info
    string infoMsg = "";
    infoMsg += "External File:\n" + EXTERNAL_GAME_CONFIG_FILE + "\n\n";
    infoMsg += "External File Version: " + PatchManager::getInstance()->getExternalConfigFileVersion() + "\n";
    infoMsg += "Internal File Version: " + INTERNAL_GAME_CONFIG_FILE_VERSION;
    
    auto infoLabel = Label::createWithTTF(infoMsg, FONT_RETRO, 28,
                                          Size(SB_WIN_SIZE.width*0.95f, 0),
                                          TextHAlignment::CENTER, TextVAlignment::TOP);
    infoLabel->setAnchorPoint(ANCHOR_MT);
    infoLabel->setPosition(Vec2TC(0, -10));
    addChild(infoLabel);
    
    // json
    // string json = SBStringUtils::readTextFile(EXTERNAL_GAME_CONFIG_FILE);
    string json = SBStringUtils::readTextFile(INTERNAL_GAME_CONFIG_FILE);
    
    /*
    auto jsonLabel = Label::createWithTTF(json, FONT_RETRO, 25,
                                          Size(SB_WIN_SIZE.width*0.95f, 0),
                                          TextHAlignment::CENTER, TextVAlignment::TOP);
    jsonLabel->setAnchorPoint(ANCHOR_MT);
    jsonLabel->setPosition(infoLabel->getPosition() +
                           Vec2(0, -infoLabel->getContentSize().height-20));
    addChild(jsonLabel);
    */
    
    auto jsonBtn = UIHelper::createFontButton("JSON", ButtonSize::MEDIUM);
    jsonBtn->setAnchorPoint(ANCHOR_M);
    jsonBtn->setPosition(Vec2MC(0, 0));
    addChild(jsonBtn);
    
    jsonBtn->setOnClickListener([=](Node*) {
        MessageBox(json.c_str(), "");
    });
    
    return true;
}
