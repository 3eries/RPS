//
//  GameConfiguration.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#include "GameConfiguration.hpp"

#include "json/document-wrapper.h"

USING_NS_CC;
using namespace std;

#define JSON_FILE       "config/game_config.json"

static GameConfiguration *instance = nullptr;
GameConfiguration* GameConfiguration::getInstance() {
    
    if( !instance ) {
        instance = new GameConfiguration();
    }
    
    return instance;
}

void GameConfiguration::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

GameConfiguration::GameConfiguration() {
}

GameConfiguration::~GameConfiguration() {
}

void GameConfiguration::init() {

    string jsonStr = SBStringUtils::readTextFile(JSON_FILE);
    CCLOG("jsonStr: %s", jsonStr.c_str());
    
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    
    // time
    {
        auto timeObj = doc["time"].GetObject();
        
        timeInfo.maxPoint = timeObj["max_point"].GetInt();
        timeInfo.firstPoint = timeObj["first_point"].GetInt();
        timeInfo.increasePointPerHit = timeObj["increase_point_per_hit"].GetInt();
    }
    
    // fever
    {
        auto feverObj = doc["fever"].GetObject();
        
        feverInfo.maxPoint = feverObj["max_point"].GetFloat();
        feverInfo.points[RSPType::ROCK] = feverObj["rock_point"].GetFloat();
        feverInfo.points[RSPType::SCISSORS] = feverObj["scissors_point"].GetFloat();
        feverInfo.points[RSPType::PAPER] = feverObj["paper_point"].GetFloat();
        feverInfo.points[RSPType::ROCK_N_ROLL] = feverObj["rock_n_roll_point"].GetFloat();
        feverInfo.gageDuration = feverObj["gage_duration"].GetFloat();
        feverInfo.duration = feverObj["duration"].GetFloat();
        feverInfo.endDelay = feverObj["end_delay"].GetFloat();
        
//        feverInfo.points[RSPType::ROCK] = 5;
//        feverInfo.points[RSPType::SCISSORS] = 5;
//        feverInfo.points[RSPType::PAPER] = 5;
    }
    
    // levels
    {
        auto levelList = doc["levels"].GetArray();
        
        for( int i = 0; i < levelList.Size(); ++i ) {
            const rapidjson::Value &levelValue = levelList[i];
            
            LevelInfo info;
            info.level = levelValue["level"].GetInt();
            info.beginRange = levelValue["begin_range"].GetInt();
            info.decreasePointPerSeconds = levelValue["decrease_point_per_sec"].GetInt();
            
            levelInfos.push_back(info);
        }
    }
}

LevelInfo GameConfiguration::getLevelInfo(int level) {
    
    if( level > getMaxLevelInfo().level ) {
        return LevelInfo();
    }
    
    return levelInfos[level-1];
}

LevelInfo GameConfiguration::getMaxLevelInfo() {
    
    if( levelInfos.size() == 0 ) {
        return LevelInfo();
    }
    
    return levelInfos[levelInfos.size()-1];
}


