//
//  UIHelper.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#ifndef GAMEUIHelper_h
#define GAMEUIHelper_h

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"
#include "RSP.h"

// 버튼 정의
enum class ButtonType {
    FONT,           // 기본 폰트
};

struct ButtonSize {
    static const cocos2d::Size SMALL;
    static const cocos2d::Size MEDIUM;
    static const cocos2d::Size LARGE;
};

/** @class UIHelper
 * @brief UI 관련 공통 기능을 정의합니다
 */
class UIHelper {
public:
    static std::string getButtonImageFile(RSPType type);
    
    static SBButton* createButton(SBButton::Config config);
    static SBButton* createFontButton(const std::string &title, const cocos2d::Size &size);
};

#endif /* GAMEUIHelper_h */
