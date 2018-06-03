//
//  ViewManager.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#include "ViewManager.hpp"

#include "MainScene.hpp"

#include "../main/MainView.hpp"
#include "../game/GameView.hpp"

USING_NS_CC;
using namespace std;

static ViewManager *instance = nullptr;
ViewManager* ViewManager::getInstance() {
    
    if( !instance ) {
        instance = new ViewManager();
    }
    
    return instance;
}

void ViewManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

ViewManager::ViewManager() :
viewType(ViewType::NONE),
mainScene(nullptr),
gameView(nullptr) {
}

ViewManager::~ViewManager() {
    
    views.clear();
    listeners.clear();
}

/**
 * View 생성
 */
Node* ViewManager::createView(ViewType type) {
    
    switch( type ) {
        case ViewType::MAIN:          return MainView::create();
        case ViewType::GAME:          return nullptr;
        case ViewType::GAME_OVER:     return nullptr;
        default:
            CCASSERT(false, "ViewManager::createView error: invalid view type.");
            break;
    }
    
    return nullptr;
}

/**
 * View 추가
 */
Node* ViewManager::addView(ViewType type, Node *view) {
    
    removeView(type);
    
    mainScene->addChild(view);
    views[type] = view;
    
    return view;
}

/**
 * View 제거
 */
bool ViewManager::removeView(ViewType type) {
    
    auto it = views.find(type);
    
    if( it != views.end() ) {
        it->second->removeFromParent();
        views.erase(it);
        
        return true;
    }
    
    return false;
}

void ViewManager::initGameView() {
    
    // 게임뷰 초기화
    if( gameView ) {
        gameView->removeFromParent();
    }
    
    gameView = GameView::create();
    mainScene->addChild(gameView);
}

/**
 * 메인 Scene 진입
 */
void ViewManager::onEnterMainScene(MainScene *mainScene) {
    
    this->mainScene = mainScene;
    this->viewType = ViewType::MAIN;
    
    replaceMainView();
}

/**
 * View 전환
 */
void ViewManager::replaceView(ViewType type, std::function<Node*()> createViewFunc) {
    
    CCASSERT(type != ViewType::NONE, "ViewManager::replaceView error: invalid view type.");
    CCASSERT(createViewFunc != nullptr, "ViewManager::replaceView error: create view function is null.");
    
    this->viewType = type;
    
    // remove other views
    for( auto t : VIEW_TYPES ) {
        if( type != t ) {
            removeView(t);
        }
    }
    
    /*
    for( auto it = views.begin(); it != views.end(); ) {
        if( it->first != type && removeView(it->first) ) {
        } else {
            ++it;
        }
    }
    */
    
    CCLOG("ViewManager::replaceView: %d, %d", (int)type, views.size());
    
    // View 생성
    auto view = createViewFunc();
//    CCASSERT(view != nullptr, "ViewManager::replaceView error: invalid view.");
    
    if( view ) {
        // View 전환 효과
        switch( type ) {
            case ViewType::MAIN:          break;
            case ViewType::GAME:          break;
            default: break;
        }
        
        // View 전환
        addView(type, view);
    }
    
    for( auto listener : listeners ) {
        listener->onViewChanged(viewType);
    }
}

void ViewManager::replaceView(ViewType type) {
    
    return replaceView(type, [=]() -> Node* {
        return this->createView(type);
    });
}

/**
 * 메인 뷰로 전환
 */
void ViewManager::replaceMainView() {
    
    initGameView();
    
    // 뷰 전환
    replaceView(ViewType::MAIN);
}

/**
 * 백키 이벤트 핸들링
 * View 공통으로 처리할 로직 구현
 */
bool ViewManager::onBackKeyReleased() {
    
    if( !mainScene || viewType == ViewType::NONE ) {
        return false;
    }
    
    auto children = mainScene->getChildren();
    
    for( auto child : children ) {
        if( false /*TODO*/) {
            return true;
        }
    }
    
    return false;
}

#pragma mark- ViewListener

void ViewManager::addListener(ViewListener *listener) {
    
    CCASSERT(listener != nullptr, "ViewManager::addListener error: invalid listener.");
    
    auto it = find(listeners.begin(), listeners.end(), listener);
    
    if( it == listeners.end() ) {
        listeners.push_back(listener);
    }
}

void ViewManager::removeListener(ViewListener *listener) {
    
    CCASSERT(listener != nullptr, "ViewManager::removeListener error: invalid listener.");
    
    auto it = find(listeners.begin(), listeners.end(), listener);
    
    if( it != listeners.end() ) {
        listeners.erase(it);
    }
}

