//
//  SBMath.cpp
//
//  Created by seongmin hwang on 2018. 9. 18..
//

#include "SBMath.hpp"

USING_NS_CC;

/**
 * 자릿수를 반환한다
 */
int SBMath::getDigit(int n) {
    
    if( n < 0 )
        n = -n;
    
    int digit = 0;
    
    for( ; n > 0; n /= 10, digit++ );
    
    return digit;
}
