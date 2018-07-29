//
//  AdsDefine.h
//
//  Created by seongmin hwang on 2018. 7. 26..
//

#ifndef AdsDefine_h
#define AdsDefine_h

#include "../../base/SBMacros.h"

NS_SB_BEGIN;

enum class AdPlatform {
    NONE = 0,
    ADMOB,
    FACEBOOK,
    INMOBI,
    UNITY,
    VUNGLE,
};

static const std::map<AdPlatform, std::string> AD_PLATFORM_STRING_MAP = {
    { AdPlatform::NONE,        "none", },
    { AdPlatform::ADMOB,       "admob", },
    { AdPlatform::FACEBOOK,    "facebook", },
    { AdPlatform::INMOBI,      "inmobi", },
    { AdPlatform::UNITY,       "unity", },
    { AdPlatform::VUNGLE,      "vungle", },
};

enum class AdType {
    NONE = 0,
    BANNER,
    INTERSTITIAL,
    REWARDED_VIDEO,
};

struct AdsConfig {
    bool testMode;
    std::vector<std::string> testDevices;
    std::string appId;
    std::string bannerUnitId;
    std::string interstitialUnitId;
    std::string rewardedVideoUnitId;
    std::map<AdPlatform, std::vector<std::string>> placements;
};

NS_SB_END;

#endif /* AdsDefine_h */
