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

static const float          FEVER_END_ALERT_TIME         = 1.0f;    // 피버 모드 종료 알림 시간, n초 전

static const float          DRAW_VIBRATE_DURATION        = 0.1f;    // 비겼을 시 진동 시간
static const float          GAME_OVER_DELAY              = 1.0f;    // 게임 오버 연출 지연 시간

static const size_t         BLOCK_COUNT                  = 11;
static const size_t         BLOCK_DISPLAY_COUNT          = 8;
static const float          BLOCK_WIDTH                  = 180;
static const float          BLOCK_HEIGHT                 = 132;

static const float          BLOCK_RANDOM_X               = 5;
static const float          BLOCK_ORIGIN_POS_Y           = 285;
static const float          BLOCK_PADDING_Y              = -4;

// 화면에 보이는 블럭 높이
static const float          BLOCK_DISPLAY_HEIGHT         = 950; // (BLOCK_HEIGHT*BLOCK_DISPLAY_COUNT)-20;

// 블럭 내려오는 시간
static const float          BLOCK_MOVE_DURATION          = 0.1f;

// 캐릭터 좌표
#define                     MAN_POS_LEFT                 Vec2BC(-212, 210)
#define                     MAN_POS_RIGHT                Vec2BC(212,  210)

#endif /* GameDefine_h */
