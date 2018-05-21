//
//  SBBasePopup.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef SBBasePopup_hpp
#define SBBasePopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../../base/SBTypes.hpp"

class SBBasePopup : public cocos2d::Node, public SBNodeListener {
public:
    SBBasePopup();
    virtual ~SBBasePopup();
    
protected:
    bool init() override;
};

#endif /* SBBasePopup_hpp */
