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

#define                  AES256_KEY                               "A1din7oih98dh9wef974fnsudh54ugJs"

static const std::string GAME_CONFIG_FILE                       = "game_config.json";

static const std::string INTERNAL_GAME_CONFIG_FILE              = "config/" + GAME_CONFIG_FILE;
#define                  EXTERNAL_GAME_CONFIG_FILE                 std::string(cocos2d::FileUtils::getInstance()->getWritablePath() + GAME_CONFIG_FILE)

static const std::string INTERNAL_GAME_CONFIG_FILE_VERSION      = "1.0.0";

// Plugin
static const std::string PLUGIN_CONFIG_FILE                     = "config/plugin_config.json";

// Firebase Analytics
static const std::string FA_SCREEN_SPLASH                       = "splash";
static const std::string FA_SCREEN_MAIN                         = "main";
static const std::string FA_SCREEN_GAME                         = "game";
static const std::string FA_SCREEN_GAME_RESULT                  = "game_result";

static const std::string FA_EVENT_GAME_PLAY                     = "game_play";             // 게임 플레이 횟수
static const std::string FA_EVENT_GAME_RESULT                   = "game_result";           // 게임 결과
static const std::string FA_EVENT_FEVER                         = "fever";                 // 피버 진입 총 횟수
static const std::string FA_EVENT_FIRST_FEVER                   = "first_fever";           // 게임 당 최초 피버 진입 횟수
static const std::string FA_EVENT_CREDIT                        = "credit";                // 크레딧 화면 진입
static const std::string FA_EVENT_SHOP_COMING_SOON              = "shop_coming_soon";      // 커밍순 샵 버튼 클릭

// Leaderboard
// 현재는 아이디를 사용하지만, 추후 plugin_config.json에 이름과 아이디를 정의하여 사용
static const std::string LEADER_BOARD_HIGH_SCORE                = "CgkIhM_O4eseEAIQAQ";

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
static const std::string DIR_IMG_GAME_IPAD          = DIR_ADD(DIR_IMG_GAME, "ipad");
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
static const std::string ANIM_CONTINUE              = DIR_ANIM + "continue" + ANIM_EXT;

static const std::string ANIM_NAME_RUN              = "run";
static const std::string ANIM_NAME_CLEAR            = "clear";
static const std::string ANIM_NAME_ENTER            = "enter";
static const std::string ANIM_NAME_EXIT             = "exit";
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
static const std::string FONT_NEW_RECORD            = DIR_FONT + "Sabo-Filled.otf";
static const std::string FONT_FAMOUS_SAYING         = DIR_FONT + "game_over.ttf";

// 사운드 경로
static const std::string DIR_SOUND                  = "sounds/";

static const std::string SOUND_BGM_MAIN             = DIR_SOUND + "bgm_main.mp3";
static const std::string SOUND_BGM_GAME             = DIR_SOUND + "bgm_game_endless_pain_of_nightmares.mp3";
static const std::string SOUND_BGM_FEVER            = DIR_SOUND + "bgm_game_fever_metal_opening.mp3";
static const std::string SOUND_BGM_CONTINUE         = DIR_SOUND + "bgm_take_your_time.mp3";
static const std::string SOUND_BGM_RECORD           = DIR_SOUND + "bgm_take_your_time.mp3";

static const std::string SOUND_BUTTON_CLICK         = DIR_SOUND + "btn_click.mp3";
static const std::string SOUND_GAME_OVER            = DIR_SOUND + "game_over.mp3";
static const std::string SOUND_THUNDER              = DIR_SOUND + "thunder.mp3";
static const std::string SOUND_SCORE_ROLLING        = DIR_SOUND + "score_rolling.mp3";
static const std::string SOUND_LIFE_UP              = DIR_SOUND + "life_up.mp3";

static const std::string SOUND_PUNCH                = DIR_SOUND + "punch_1.mp3";
static const std::vector<std::string> SOUND_PUNCH_VOICE({
    DIR_SOUND + "kung_fu_voice_1.mp3",
    DIR_SOUND + "kung_fu_voice_2.mp3",
    DIR_SOUND + "kung_fu_voice_3.mp3",
    DIR_SOUND + "kung_fu_voice_4.mp3",
});

// 좌표
#define TOP_MENU_MARGIN_Y       (-62 + ((!SceneManager::isGameScene() || User::isOwnRemoveAdsItem()) ? 0 : -superbomb::AdsHelper::getBannerHeight()))

// Color
namespace Color {
    static const cocos2d::Color4B POPUP_BG          = cocos2d::Color4B(0,0,0,255*0.75f);
}

// Popup
namespace PopupZOrder {
    static const int BOTTOM = SBZOrder::MIDDLE;
    static const int MIDDLE = BOTTOM + 1;
    static const int TOP    = BOTTOM + 2;
}

// 연출 시간
namespace EffectDuration {
    static const float POPUP_SLIDE_SLOW        = 0.4f;
    static const float POPUP_SLIDE_NORMAL      = 0.3f;
    static const float POPUP_SLIDE_FAST        = 0.25f;
    
    static const float MENU_SLIDE_NORMAL       = 0.3f;
    static const float MENU_SLIDE_FAST         = 0.25f;
}

/////////////////////
// 공통 함수
/////////////////////
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

static RSPType getLoseHand(RSPType hand) {
    
    switch( hand ) {
        case RSPType::ROCK:            return RSPType::SCISSORS;
        case RSPType::SCISSORS:        return RSPType::PAPER;
        case RSPType::PAPER:           return RSPType::ROCK;
        default:
            CCASSERT(false, "getLoseHand error: invalid hand type.");
            break;
    }
    
    return RSPType::NONE;
}

static RSPType getRandomHand() {
    
    int ran = arc4random() % 3;
    return (RSPType)(ran + (int)RSPType::ROCK);
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
