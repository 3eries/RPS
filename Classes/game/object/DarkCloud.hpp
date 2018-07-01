//
//  DarkCloud.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 28..
//

#ifndef DarkCloud_hpp
#define DarkCloud_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../GameManager.hpp"

class TimeBar;

/** @class DarkCloud
 * @brief 먹구름
 */
class DarkCloud : public cocos2d::Node, public GameListener {
private:
    static const int MOVE_ACTION_TAG_NONE = -1;
    static const int MOVE_ACTION_TAG      = 100;
    static const int MOVE_ACTION_TAG_UP   = 100;
    static const int MOVE_ACTION_TAG_DOWN = 200;
    
private:
    enum ActionTag {
        MOVE = 100,
        FADE,
    };
    
public:
    CREATE_FUNC(DarkCloud);
    ~DarkCloud();
    
private:
    DarkCloud();
    
    bool init() override;
    void onExit() override;
    
    void reset();
    
// GameListener
private:
    void onGameStart() override;
    void onGameRestart() override;
    void onGamePause() override;
    void onGameResume() override;
    void onPreGameOver() override;
    void onContinue() override;
    void onGameOver() override;
    void onStartTimer() override;
    
public:
    void updateCloud(float timeRatio);
    void updateCloudPosition(float timeRatio);
    void updateCloudOpacity(float timeRatio);
    
    float getCloudPositionY(float timeRatio);
    float getCloudOpacity(float timeRatio);
    
private:
    CC_SYNTHESIZE(TimeBar*, timeBar, TimeBar);
    
    float timeRatio;
    bool isPositionLocked;      // 좌표 변경 잠금 여부
    bool isOpacityLocked;       // 투명도 변경 잠금 여부
    
    spine::SkeletonAnimation *anim;
    int actionTag;
    int section;
};

#endif /* DarkCloud_hpp */
