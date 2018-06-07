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

static const std::string SERVER_VERSION_FILE                    = "http://superbomb.co/3eries/rsp/version.json";
static const std::string SERVER_GAME_CONFIG_FILE                = "http://superbomb.co/3eries/rsp/game_config.json";

static const std::string GAME_CONFIG_FILE                       = "game_config.json";

static const std::string INTERNAL_GAME_CONFIG_FILE              = "config/" + GAME_CONFIG_FILE;
#define                  EXTERNAL_GAME_CONFIG_FILE                 std::string(cocos2d::FileUtils::getInstance()->getWritablePath() + GAME_CONFIG_FILE)

static const std::string INTERNAL_GAME_CONFIG_FILE_VERSION      = "1.0.0";

// 이미지 경로
static const std::string DIR_IMG                    = "images/";
static const std::string DIR_IMG_COMMON             = DIR_ADD(DIR_IMG, "common");
static const std::string DIR_IMG_SPLASH             = DIR_ADD(DIR_IMG, "splash");
static const std::string DIR_IMG_GAME               = DIR_ADD(DIR_IMG, "game");
static const std::string DIR_IMG_MAIN               = DIR_ADD(DIR_IMG, "main");

static const std::string DIR_ANIM                   = "anims/";

// 폰트 경로
static const std::string DIR_FONT                   = "fonts/";

static const std::string FONT_RETRO                 = DIR_FONT + "Commodore.ttf";

// 사운드 경로
static const std::string DIR_SOUND                  = "sounds/";

static const std::string SOUND_BGM_MAIN             = DIR_SOUND + "bgm_main.mp3";
static const std::string SOUND_BGM_GAME             = DIR_SOUND + "bgm_game.mp3";
static const std::string SOUND_BGM_FEVER            = DIR_SOUND + "bgm_fever.mp3";
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
    ROCK_N_ROLL,
};

enum class RSPResult {
    WIN,
    LOSE,
    DRAW,
};

static RSPResult getResult(RSPType myHand, RSPType oppHand) {
    
    CCASSERT(myHand != RSPType::NONE, "getResult error: invalid my hand.");
    CCASSERT(oppHand != RSPType::NONE, "getResult error: invalid opp hand.");
    
    if( myHand == oppHand ) {
        return RSPResult::DRAW;
    }
    
    if( myHand == RSPType::ROCK_N_ROLL ||
        oppHand == RSPType::ROCK_N_ROLL ) {
        // 락앤롤
        return RSPResult::WIN;
    }
    
    bool isWin = false;
    
    switch( oppHand ) {
        case RSPType::ROCK:            isWin = (myHand == RSPType::PAPER);      break;
        case RSPType::SCISSORS:        isWin = (myHand == RSPType::ROCK);       break;
        case RSPType::PAPER:           isWin = (myHand == RSPType::SCISSORS);   break;
        case RSPType::ROCK_N_ROLL:     isWin = true;                            break;
        default: break;
    }
    
    if( isWin ) {
        return RSPResult::WIN;
    } else {
        return RSPResult::LOSE;
    }
}

#endif /* RSP_h */
