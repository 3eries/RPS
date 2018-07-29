//
//  PluginManager.cpp
//
//  Created by seongmin hwang on 2018. 7. 20..
//

#include "PluginManager.hpp"

#include "json/document-wrapper.h"
#include "../utils/SBStringUtils.h"

#include "ads/AdsHelper.hpp"
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

void PluginManager::init(const string &jsonFile) {
 
    string json = SBStringUtils::readTextFile(jsonFile);
    CCLOG("PluginManager::init:\n%s", json.c_str());
    
    // parse json
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto obj = doc["ios"].GetObject();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto obj = doc["android"].GetObject();
#endif
    
    PluginPlay::getInstance()->init();
    FirebaseManager::getInstance()->init();
    
    // ads
    {
        auto adObj = obj["ads"].GetObject();
        
        AdsConfig adsConfig;
        
        // test value
        {
            adsConfig.testMode = (adObj["test"].GetInt() != 0);
            
            auto testDevices = adObj["test_devices"].GetArray();
            
            for( int i = 0; i < testDevices.Size(); ++i ) {
                adsConfig.testDevices.push_back(testDevices[i].GetString());
            }
        }
        
        // app id
        adsConfig.appId = adObj["app_id"].GetString();
        
        // ad unit id
        if( adsConfig.testMode ) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            // Banner                        ca-app-pub-3940256099942544/2934735716
            // Interstitial                  ca-app-pub-3940256099942544/4411468910
            // Interstitial Video            ca-app-pub-3940256099942544/5135589807
            // Rewarded Video                ca-app-pub-3940256099942544/1712485313
            // Native Advanced               ca-app-pub-3940256099942544/3986624511
            // Native Advanced Video         ca-app-pub-3940256099942544/2521693316
            adsConfig.bannerUnitId        = "ca-app-pub-3940256099942544/2934735716";
            adsConfig.interstitialUnitId  = "ca-app-pub-3940256099942544/4411468910";
            adsConfig.rewardedVideoUnitId = "ca-app-pub-3940256099942544/1712485313";
            
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            // Banner                        ca-app-pub-3940256099942544/6300978111
            // Interstitial                  ca-app-pub-3940256099942544/1033173712
            // Interstitial Video            ca-app-pub-3940256099942544/8691691433
            // Rewarded Video                ca-app-pub-3940256099942544/5224354917
            // Native Advanced               ca-app-pub-3940256099942544/2247696110
            // Native Advanced Video         ca-app-pub-3940256099942544/1044960115
            adsConfig.bannerUnitId        = "ca-app-pub-3940256099942544/6300978111";
            adsConfig.interstitialUnitId  = "ca-app-pub-3940256099942544/1033173712";
            adsConfig.rewardedVideoUnitId = "ca-app-pub-3940256099942544/5224354917";
#endif
            
        } else {
            adsConfig.bannerUnitId = adObj["banner_unit_id"].GetString();
            adsConfig.interstitialUnitId = adObj["interstitial_unit_id"].GetString();
            adsConfig.rewardedVideoUnitId = adObj["rewarded_video_unit_id"].GetString();
        }
        
        // placements
        auto placements = adObj["placements"].GetObject();
        
        auto getPlatformPlacements = [=](const string &key) -> vector<string> {
            
            vector<string> platformPlacements;
            
            if( placements.HasMember(key.c_str()) ) {
                auto array = placements[key.c_str()].GetArray();

                for( int i = 0; i < array.Size(); ++i ) {
                    platformPlacements.push_back(array[i].GetString());
                }
            }
            
            return platformPlacements;
        };

        const AdPlatform PLATFORMS[] = {
            AdPlatform::VUNGLE,
        };
        
        for( auto platform : PLATFORMS ) {
            auto placements = getPlatformPlacements(AD_PLATFORM_STRING_MAP.at(platform));
            adsConfig.placements[platform] = placements;
        }
        
        AdsHelper::getInstance()->init(adsConfig);
    }
}

NS_SB_END;
