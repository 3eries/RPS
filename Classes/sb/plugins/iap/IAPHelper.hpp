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

#include "IAPListener.hpp"

NS_SB_BEGIN;

class IAPHelper {
public:
    static IAPHelper* getInstance();
    static void destroyInstance();
    ~IAPHelper();
    
private:
    IAPHelper();
    
public:
    void init();
    
public:
    void addListener(IAPListener *listener);
    void addListener(cocos2d::Node *target, IAPListener *listener);
    
    void removeListener(IAPListener *listener);
    void removeListener(cocos2d::Node *target);
    
    void onRemoveAdsPurchased();
    
private:
    cocos2d::Vector<IAPListener*> listeners;
};

NS_SB_END;

#endif /* IAPHelper_hpp */
