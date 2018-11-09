//
//  superbomb.h
//
//  Created by hwangseongmin on 2016. 11. 24..
//
//

#ifndef superbomb_h
#define superbomb_h

// base
#include "base/SBDirector.hpp"
#include "base/SBMacros.h"
#include "base/SBScheduleHelper.hpp"
#include "base/SBTypes.hpp"
#include "base/SBUserDefaultKey.h"

// consent
#include "consent/ConsentForm.hpp"
#include "consent/ConsentManager.hpp"

// audio
#include "audio/SBAudioEngine.hpp"

// effect
#include "effect/SBAction.hpp"
#include "effect/SBActionHelper.hpp"
#include "effect/SBEffect.h"

// json
#include "json/SBJSON.h"

// network
#include "network/SBHttp.hpp"

// ui
#include "ui/SBUIInfo.hpp"
#include "ui/widgets/SBAnimationSprite.hpp"
#include "ui/widgets/SBBasePopup.hpp"
#include "ui/widgets/SBButton.hpp"
#include "ui/widgets/SBToggleButton.hpp"

// utils
#include "utils/SBCollection.hpp"
#include "utils/SBMath.hpp"
#include "utils/SBNodeUtils.hpp"
#include "utils/SBStringUtils.h"
#include "utils/SBSystemUtils.h"
#include "utils/ReviewHelper.hpp"

// editor-support
#include "editor-support/spine/SBSpineHelper.hpp"

// platform
#include "platform/SBPlatformHelper.h"
#include "platform/SBPlatformMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/SBJniHelper.hpp"

#endif

// security
#include "security/SBSecurity.hpp"

// plugins
#include "plugins/plugins.h"

#endif /* superbomb_h */
