//
//  FunnyHelper.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 7. 6..
//

#include "FunnyHelper.hpp"

using namespace std;

// 명언 목록
static const vector<FamousSaying> FAMOUS_SAYING_LIST = {
    FamousSaying("To be, or not to be,\nthat is the question.", "William Shakespeare"),
    FamousSaying("Simple is the Best.", "B.S.J"),
    FamousSaying("Simplicity is\nthe ultimate sophistication.", "Leonardo da Vinci"),
};

FamousSaying FunnyHelper::getRandomFamousSaying() {
    
    int ran = arc4random() % FAMOUS_SAYING_LIST.size();
    return FAMOUS_SAYING_LIST[ran];
}
