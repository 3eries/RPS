//
//  ViewManager.hpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#ifndef ViewManager_hpp
#define ViewManager_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class MainScene;
class GameView;

enum class ViewType {
    NONE,
    MAIN,
    GAME,
    GAME_OVER,
};

static const ViewType VIEW_TYPES[] = {
    ViewType::MAIN,
    ViewType::GAME,
};

#pragma mark- ViewListener

class ViewListener {
public:
    virtual void onViewChanged(ViewType viewType) {};      // 뷰 상태 변경
};

#pragma mark- ViewManager

class ViewManager : public cocos2d::Ref {
public:
    static ViewManager* getInstance();
    static void destroyInstance();
    
    ~ViewManager();
    
private:
    ViewManager();
    
    cocos2d::Node* createView(ViewType type);
    cocos2d::Node* addView(ViewType type, cocos2d::Node *view);
    bool           removeView(ViewType type);
    
public:
    void initGameView();
    
    void onEnterMainScene(MainScene *mainScene);
    
    void replaceView(ViewType type, std::function<cocos2d::Node*()> createViewFunc);
    void replaceView(ViewType type);
    void replaceMainView();
    
    bool onBackKeyReleased();
    
private:
    CC_SYNTHESIZE_READONLY(ViewType, viewType, ViewType);
    CC_SYNTHESIZE_READONLY(MainScene*, mainScene, MainScene);
    
    std::map<ViewType, cocos2d::Node*> views;

    // 게임뷰는 모든 화면의 기본 배경이 되기 때문에 항상 존재한다
    GameView *gameView;
    
// ViewListener
public:
    void addListener(ViewListener *listener);
    void removeListener(ViewListener *listener);
    
private:
    std::vector<ViewListener*> listeners;
};

#endif /* ViewManager_hpp */
