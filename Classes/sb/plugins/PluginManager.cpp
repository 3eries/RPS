//
//  PluginManager.cpp
//
//  Created by seongmin hwang on 2018. 7. 20..
//

#include "PluginManager.hpp"

#include "firebase/FirebaseManager.hpp"
#include "iap/IAPHelper.hpp"
#include "play/PluginPlay.hpp"

USING_NS_CC;
using namespace std;

NS_SB_BEGIN;

static PluginManager *instance = nullptr;
PluginManager* PluginManager::getInstance() {
    
    if( !instance ) {
        instance = new PluginManager();
    }
    
    return instance;
}

void PluginManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
}

void PluginManager::init() {
    
    CCLOG("PluginManager::init");
 
    PluginPlay::getInstance()->init();
    FirebaseManager::getInstance()->init();
}

NS_SB_END;
