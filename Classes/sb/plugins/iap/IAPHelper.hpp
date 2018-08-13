//
//  IAPHelper.hpp
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#ifndef IAPHelper_hpp
#define IAPHelper_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../../base/SBMacros.h"
#include "../../base/SBTypes.hpp"

#include "IAPDefine.h"
#include "IAPListener.hpp"

NS_SB_BEGIN;
NS_IAP_BEGIN;

class IAPHelper {
public:
    enum class State {
        IDLE,       // 대기
        PURCHASE,   // 결제
        RESTORE,    // 복원
    };
    
public:
    static IAPHelper* getInstance();
    static void destroyInstance();
    ~IAPHelper();
    
private:
    IAPHelper();
    
    static void purchaseImpl(const Item &item);
    static void restoreImpl();
    
public:
    void init(const Config &config, const std::string &originalJson = "");
    void initImpl(const Config &config, const std::string &originalJson);
    
    /**
     * Enable/disable debug logging
     */
    static void setDebug(bool debug);
    
    /**
     * Get all the products
     */
    static std::vector<Item> getItems();
    
    static Item getItemByName(const std::string &name);
    static Item getItemById(const std::string &itemId);
    static Item getRemoveAdsItem();
    
    /**
     * Make a purchase request
     *
     * @Param name is the name of the item specified in plugin_config.json
     */
    static bool purchase(const std::string &name, PurchaseListener *listener);
    static bool purchaseRemoveAds(PurchaseListener *listener);
    
    /**
     * Restore purchase
     */
    static void restore(RestoreListener *listener);
    
public:
    void onPurchased(const std::string &itemId);
    void onPurchaseError(const std::string &errorMsg = "");
    void onPurchaseCanceled();
    
    void onRestored(const std::string &itemId);
    void onRestoreError(const std::string &errorMsg = "");
    
public:
    void addListener(Listener *listener);
    void addListener(cocos2d::Ref *target, Listener *listener);
    
    void removeListener(Listener *listener);
    void removeListener(cocos2d::Ref *target);
    void removeListeners(std::vector<Listener*> listeners, bool remainForever = true);
    void removeListeners(cocos2d::Vector<Listener*> listeners, bool remainForever = true);
//    void removeListeners(std::vector<PurchaseListener*> listeners, bool remainForever = true);
    
    std::vector<PurchaseListener*> getPurchaseListeners();
    std::vector<RestoreListener*> getRestoreListeners();
    
protected:
    State state;
    
    Config config;
    cocos2d::Vector<Listener*> listeners;
};

NS_IAP_END;
NS_SB_END;

#endif /* IAPHelper_hpp */
