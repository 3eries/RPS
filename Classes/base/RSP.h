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

// 가위바위보 타입
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

// 이미지 경로
static const std::string DIR_IMG                    = "images/";
static const std::string DIR_IMG_COMMON             = DIR_ADD(DIR_IMG, "common");
static const std::string DIR_IMG_SPLASH             = DIR_ADD(DIR_IMG, "splash");
static const std::string DIR_IMG_GAME               = DIR_ADD(DIR_IMG, "game");
static const std::string DIR_IMG_MAIN               = DIR_ADD(DIR_IMG, "main");

// 애니메이션 경로
static const std::string DIR_ANIM                   = "anims/";

// 스파인 애니메이션
static const std::string ANIM_EXT                   = ".json";
static const std::string ANIM_LOGO                  = DIR_ANIM + "3eries_logo" + ANIM_EXT;
static const std::string ANIM_CLOUD                 = DIR_ANIM + "RSP_cloud" + ANIM_EXT;
static const std::string ANIM_CLOUD_DARK            = DIR_ANIM + "cloud_dark" + ANIM_EXT;
static const std::string ANIM_FEVER_MODE_BG         = DIR_ANIM + "fevermode_bg" + ANIM_EXT;
static const std::string ANIM_FEVER_MODE_FIRE       = DIR_ANIM + "fevermode_fire" + ANIM_EXT;
static const std::string ANIM_DRAW                  = DIR_ANIM + "character_draw" + ANIM_EXT;
static const std::string ANIM_DIE                   = DIR_ANIM + "die" + ANIM_EXT;

static const std::string ANIM_NAME_RUN              = "run";
static const std::string ANIM_NAME_CLEAR            = "clear";
static const std::string ANIM_NAME_DIE_LEFT         = "die_left";
static const std::string ANIM_NAME_DIE_RIGHT        = "die_right";

static std::map<RSPType, std::string> ANIM_NAME_DRAW_LEFT({
    { RSPType::ROCK,     "draw_left_rock" },
    { RSPType::SCISSORS, "draw_left_scissors" },
    { RSPType::PAPER,    "draw_left_paper" },
});
static std::map<RSPType, std::string> ANIM_NAME_DRAW_RIGHT({
    { RSPType::ROCK,     "draw_right_rock" },
    { RSPType::SCISSORS, "draw_right_scissors" },
    { RSPType::PAPER,    "draw_right_paper" },
});

static const std::string ANIM_EVENT_DIE             = "die";
static const std::string ANIM_EVENT_MAN_MOVE        = "move";

// 폰트 경로
static const std::string DIR_FONT                   = "fonts/";

static const std::string FONT_RETRO                 = DIR_FONT + "Commodore.ttf";

// 사운드 경로
static const std::string DIR_SOUND                  = "sounds/";

static const std::string SOUND_BGM_MAIN             = DIR_SOUND + "bgm_main.mp3";
static const std::string SOUND_BGM_GAME             = DIR_SOUND + "bgm_game.mp3";
static const std::string SOUND_BGM_FEVER            = DIR_SOUND + "bgm_fever.mp3";
static const std::string SOUND_PUNCH                = DIR_SOUND + "punch.mp3";
static const std::string SOUND_GAME_OVER            = DIR_SOUND + "game_over.mp3";
static const std::string SOUND_THUNDER              = DIR_SOUND + "thunder.mp3";

// Color
namespace Color {
    static const cocos2d::Color4B POPUP_BG          = cocos2d::Color4B(0,0,0,255*0.75f);
}


////////////
// 공통 함수
////////////
static RSPType getWinHand(RSPType hand) {

    switch( hand ) {
        case RSPType::ROCK:            return RSPType::PAPER;
        case RSPType::SCISSORS:        return RSPType::ROCK;
        case RSPType::PAPER:           return RSPType::SCISSORS;
        default:
            CCASSERT(false, "getWinHand error: invalid hand type.");
            break;
    }
    
    return RSPType::NONE;
}

static RSPResult getResult(RSPType myHand, RSPType oppHand) {
    
    CCASSERT(myHand != RSPType::NONE, "getResult error: invalid my hand.");
    CCASSERT(oppHand != RSPType::NONE, "getResult error: invalid opp hand.");
    
    // 락앤롤
    if( myHand == RSPType::ROCK_N_ROLL ||
       oppHand == RSPType::ROCK_N_ROLL ) {
        return RSPResult::WIN;
    }
    
    if( myHand == oppHand ) {
        return RSPResult::DRAW;
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

static std::string typeToString(RSPType type) {

    switch( type ) {
        case RSPType::ROCK:            return "ROCK";
        case RSPType::SCISSORS:        return "SCISSORS";
        case RSPType::PAPER:           return "PAPER";
        case RSPType::ROCK_N_ROLL:     return "ROCK_N_ROLL";
        default:
            return "invalid";
    }
}

#endif /* RSP_h */
