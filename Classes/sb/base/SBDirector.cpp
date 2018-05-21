//
//  SBDirector.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "SBDirector.hpp"

#include "../audio/SBAudioEngine.hpp"

USING_NS_CC;
using namespace std;

void SBDirector::init() {
    
    SBAudioEngine::getInstance()->init();
}

void SBDirector::end() {

    SBAudioEngine::getInstance()->destroyInstance();
}
