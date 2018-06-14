//
//  SBSpineHelper.hpp
//
//  Created by seongmin hwang on 2018. 6. 15..
//

#ifndef SBSpineHelper_hpp
#define SBSpineHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>
#include "../../base/SBTypes.hpp"

class SBSpineHelper {
public:
    typedef spine::SkeletonAnimation SpineAnimation;
    
public:
    static void runAnimation(SBCallback completeListener,
                             SpineAnimation *anim,
                             const std::string &animName);
    static void clearAnimation(SpineAnimation *anim, std::string clearAnimName = "");
};

#endif /* SBSpineHelper_hpp */
