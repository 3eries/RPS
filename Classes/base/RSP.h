//
//  RSP.h
//  RSP
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#ifndef RSP_h
#define RSP_h

#include "cocos2d.h"
#include "superbomb.h"

// 이미지 경로
static const std::string DIR_IMG                    = "images/";
static const std::string DIR_IMG_COMMON             = DIR_ADD(DIR_IMG, "common");
static const std::string DIR_IMG_GAME               = DIR_ADD(DIR_IMG, "game");
static const std::string DIR_IMG_MAIN               = DIR_ADD(DIR_IMG, "main");

// 폰트 경로
static const std::string DIR_FONT                   = "fonts/";

static const std::string FONT_RETRO                 = DIR_FONT + "Commodore.ttf";

// 사운드 경로
static const std::string DIR_SOUND                  = "sounds/";

static const std::string SOUND_BGM_MAIN             = DIR_SOUND + "bgm_main.mp3";
static const std::string SOUND_BGM_GAME             = DIR_SOUND + "bgm_game.mp3";
static const std::string SOUND_PUNCH                = DIR_SOUND + "punch.wav";
static const std::string SOUND_GAME_OVER            = DIR_SOUND + "game_over.mp3";

// Color
namespace Color {
    static const cocos2d::Color4B POPUP_BG          = cocos2d::Color4B(0,0,0,255*0.75f);
}

enum class RSPType {
    NONE = -1,
    ROCK,
    SCISSORS,
    PAPER,
};

#endif /* RSP_h */
