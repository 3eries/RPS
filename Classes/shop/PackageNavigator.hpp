//
//  PackageNavigator.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 10. 18..
//

#ifndef PackageNavigator_hpp
#define PackageNavigator_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "CharacterManager.hpp"

class NavigationItem;

class PackageNavigator : public cocos2d::ui::Widget {
public:
    static PackageNavigator* create();
    ~PackageNavigator();
    
private:
    PackageNavigator();
    
    bool init() override;
    void onEnter() override;
    
    void initItems();
    
private:
    void onPackageChanged(size_t i);
    void onPackagePurchase(size_t i);
    
    void onScrollEvent(cocos2d::ui::ScrollView::EventType type);
    
    void showArrows();
    
private:
    CC_SYNTHESIZE(PackageListener, onPackageChangedListener, OnPackageChangedListener);
    CC_SYNTHESIZE(PackageListener, onPackagePurchaseListener, OnPackagePurchaseListener);
    
    std::vector<Package> packs;
    
    cocos2d::ui::PageView *pageView;
    size_t pageIndex;
    int    pageDiffCount;
};

#pragma mark- NavigationItem

class NavigationItem : public cocos2d::ui::Widget {
public:
    static NavigationItem* create(Package pack);
    ~NavigationItem();
    
private:
    NavigationItem(Package pack);
    
    bool init() override;
    void onEnter() override;
    
public:
    enum class ArrowType {
        LEFT,
        RIGHT
    };
    
    void showArrow(ArrowType type);
    void hideArrow(ArrowType type);
    void hideArrows(bool isAction = true);
    
private:
    CC_SYNTHESIZE(SBCallbackNode, onPurchaseListener, OnPurchaseListener);
    Package pack;
    
    std::map<ArrowType, cocos2d::Sprite*> arrows;
};

#endif /* PackageNavigator_hpp */
