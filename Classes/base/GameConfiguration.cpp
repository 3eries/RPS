//
//  GameConfiguration.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#include "GameConfiguration.hpp"

#include "json/document-wrapper.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

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

GameConfiguration::GameConfiguration() :
continuation(0) {
}

GameConfiguration::~GameConfiguration() {
}

void GameConfiguration::init() {

//    string jsonStr = SBStringUtils::readTextFile(JSON_FILE);
//    CCLOG("jsonStr: %s", jsonStr.c_str());
}

static string test(const string &json) {
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
    int continuation = doc["continuation"].GetInt();
    
    // time
    {
        auto timeObj = doc["time"].GetObject();
        
//        timeInfo.maxPoint = timeObj["max_point"].GetFloat();
//        timeInfo.firstPoint = timeObj["first_point"].GetFloat();
//        timeInfo.increasePointPerHit = timeObj["increase_point_per_hit"].GetFloat();
//        timeInfo.drawDelay = timeObj["draw_delay"].GetFloat();
    }
    
    // fever
    {
        auto feverObj = doc["fever"].GetObject();
        
//        feverInfo.maxPoint = feverObj["max_point"].GetFloat();
//        feverInfo.points[RSPType::ROCK] = feverObj["rock_point"].GetFloat();
//        feverInfo.points[RSPType::SCISSORS] = feverObj["scissors_point"].GetFloat();
//        feverInfo.points[RSPType::PAPER] = feverObj["paper_point"].GetFloat();
//        feverInfo.points[RSPType::ROCK_N_ROLL] = feverObj["rock_n_roll_point"].GetFloat();
//        feverInfo.gageDuration = feverObj["gage_duration"].GetFloat();
//        feverInfo.duration = feverObj["duration"].GetFloat();
//        feverInfo.endDelay = feverObj["end_delay"].GetFloat();
        
        //        feverInfo.points[RSPType::ROCK] = 5;
        //        feverInfo.points[RSPType::SCISSORS] = 5;
        //        feverInfo.points[RSPType::PAPER] = 5;
    }
    
    // levels
//    levelInfos.clear();
    
    {
        auto levelList = doc["levels"].GetArray();
        
        for( int i = 0; i < levelList.Size(); ++i ) {
            rapidjson::Value &levelValue = levelList[i];
            float decreasePointPerSeconds = levelValue["decrease_point_per_sec"].GetFloat();
            
            levelValue["decrease_point_per_sec"] = rapidjson::Value(decreasePointPerSeconds * 1.14f);
            
            
//            LevelInfo info;
//            info.level = levelValue["level"].GetInt();
//            info.beginRange = levelValue["begin_range"].GetInt();
//            info.decreasePointPerSeconds = levelValue["decrease_point_per_sec"].GetFloat();
//
//            if( levelValue.HasMember("continuation") ) {
//                info.continuation = levelValue["continuation"].GetInt();
//            } else {
//                info.continuation = defaultContinuation;
//            }
//
//            levelInfos.push_back(info);
        }
        
        doc["levels"] = rapidjson::Value(levelList);
    }
    
    rapidjson::StringBuffer strbuf;
    strbuf.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    string newJson = strbuf.GetString();
    CCLOG("newJson:\n%s", newJson.c_str());
    
    return newJson;
}

void GameConfiguration::parse(const string &json) {
    
//    test(json);
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto platformObj = doc["ios"].GetObject();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto platformObj = doc["android"].GetObject();
#endif
    
    // store
    storeUrl = platformObj["store"].GetString();
    
    // game value
    maxScore = doc["max_score"].GetInt();
    continueCondition = doc["continue_condition"].GetInt();
    continuation = doc["continuation"].GetInt();
    
    // time
    {
        auto timeObj = doc["time"].GetObject();
        
        timeInfo.maxPoint = timeObj["max_point"].GetFloat();
        timeInfo.firstPoint = timeObj["first_point"].GetFloat();
        timeInfo.continuePoint = timeObj["continue_point"].GetFloat();
        timeInfo.increasePointPerHit = timeObj["increase_point_per_hit"].GetFloat();
        timeInfo.drawDelay = timeObj["draw_delay"].GetFloat();
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
    levelInfos.clear();
    
    {
        auto levelList = doc["levels"].GetArray();
        
        for( int i = 0; i < levelList.Size(); ++i ) {
            const rapidjson::Value &levelValue = levelList[i];
            
            LevelInfo info;
            info.level = levelValue["level"].GetInt();
            info.beginRange = levelValue["begin_range"].GetInt();
            info.decreasePointPerSeconds = levelValue["decrease_point_per_sec"].GetFloat();
//            info.decreasePointPerSeconds = 25;
            
            // increase_point_per_hit
            if( levelValue.HasMember("increase_point_per_hit") ) {
                info.increasePointPerHit = levelValue["increase_point_per_hit"].GetFloat();
            } else {
                info.increasePointPerHit = timeInfo.increasePointPerHit;
            }
            
            // increase_point_per_fever_hit
            if( levelValue.HasMember("increase_point_per_fever_hit") ) {
                info.increasePointPerFeverHit = levelValue["increase_point_per_fever_hit"].GetFloat();
            } else {
                info.increasePointPerFeverHit = info.increasePointPerHit;
            }
            
            // fever_gage_rate
            if( levelValue.HasMember("fever_gage_rate") ) {
                info.feverGageRate = levelValue["fever_gage_rate"].GetFloat();
            }
            
            // gage_duration
            if( levelValue.HasMember("gage_duration") ) {
                info.gageDuration = levelValue["gage_duration"].GetFloat();
            } else {
                info.gageDuration = feverInfo.gageDuration;
            }
            
            // continuation
            if( levelValue.HasMember("continuation") ) {
                info.continuation = levelValue["continuation"].GetInt();
            } else {
                info.continuation = continuation;
            }
            
            // CCLOG("info level: %d gage_rate: %f", info.level, info.feverGageRate);
            
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


