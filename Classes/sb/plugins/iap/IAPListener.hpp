//
//  IAPListener.hpp
//  RPS-mobile
//
//  Created by seongmin hwang on 2018. 7. 12..
//

#ifndef IAPListener_hpp
#define IAPListener_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "../../base/SBMacros.h"
#include "../../base/SBTypes.hpp"

#include "IAPDefine.h"

NS_SB_BEGIN;

class IAPListener : public cocos2d::Ref {
public:
    SB_REF_CREATE_FUNC(IAPListener);
    virtual ~IAPListener();
    
    std::function<void(const Product &prod)> onPurchased;               // 구매 성공
    std::function<void(const Product &prod)> onFailure;                 // 구매 실패
    std::function<void(const Product &prod)> onCanceled;                // 구매 취소
    
    std::function<void(const Product &prod)> onRemoveAdsPurchased;      // 광고 제거 아이템 구매 성공
    
    std::function<void(const Product &prod)> onRestored;                // 아이템 복원 성공
    std::function<void(bool/*result*/)> onRestoreFinished;              // 아이템 복원 종료
    
private:
    IAPListener();
    
    CC_SYNTHESIZE(cocos2d::Node*, target, Target);
};

NS_SB_END;

#endif /* IAPListener_hpp */
