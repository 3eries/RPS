//
//  GameDefine.h
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef GameDefine_h
#define GameDefine_h

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

static const float          FEVER_END_ALERT_TIME         = 1.0f;

static const int            BANNER_HEIGHT                = 100;

static const size_t         BLOCK_COUNT                  = 11;
static const size_t         BLOCK_DISPLAY_COUNT          = 8;
static const float          BLOCK_WIDTH                  = 180;
static const float          BLOCK_HEIGHT                 = 132;

static const float          BLOCK_RANDOM_X               = 5;
static const float          BLOCK_ORIGIN_POS_Y           = 285;
static const float          BLOCK_PADDING_Y              = -4;

// 화면에 보이는 블럭 높이
static const float          BLOCK_DISPLAY_HEIGHT         = 950; // (BLOCK_HEIGHT*BLOCK_DISPLAY_COUNT)-20;

static const float          BLOCK_MOVE_DURATION          = 0.1f;

#define                     MAN_POS_LEFT                 Vec2BC(-212, 210)
#define                     MAN_POS_RIGHT                Vec2BC(212,  210)

#endif /* GameDefine_h */
