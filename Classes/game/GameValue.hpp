//
//  GameValue.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameValue_hpp
#define GameValue_hpp

#include <stdio.h>

#include "cocos2d.h"

static const float          COMBO_INTERVAL               = 0.8f;        // 콤보 간격 시간

static const size_t         BLOCK_COUNT                  = 15;
static const size_t         BLOCK_DISPLAY_COUNT          = 6;
static const float          BLOCK_WIDTH                  = 216;
static const float          BLOCK_HEIGHT                 = 184;

static const float          BLOCK_ORIGIN_POS_Y           = 330;
static const float          BLOCK_PADDING_Y              = 0;

// 화면에 보이는 블럭 높이
static const float          BLOCK_DISPLAY_HEIGHT         = 950; // (BLOCK_HEIGHT*BLOCK_DISPLAY_COUNT)-20;

static const float          BLOCK_MOVE_DURATION          = 0.1f;

#endif /* GameValue_hpp */
