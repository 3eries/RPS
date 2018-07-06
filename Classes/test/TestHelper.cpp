//
//  TestHelper.cpp
//  RSP-mobile
//
//  Created by seongmin hwang on 2018. 6. 1..
//

#include "TestHelper.hpp"

USING_NS_CC;
using namespace std;

static TestHelper *instance = nullptr;
TestHelper* TestHelper::getInstance() {
    
    if( !instance ) {
        instance = new TestHelper();
    }
    
    return instance;
}

void TestHelper::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

TestHelper::TestHelper() :
cheatMode(false),
timeBarEnabled(true),
darkCloudEnabled(true) {
}

TestHelper::~TestHelper() {
    
}
