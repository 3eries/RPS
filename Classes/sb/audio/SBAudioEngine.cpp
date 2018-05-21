//
//  SBAudioEngine.cpp
//
//  Created by seongmin hwang on 2018. 3. 13..
//

#include "SBAudioEngine.hpp"

#include "../base/SBUserDefaultKey.h"

USING_NS_CC;
using namespace cocos2d::experimental;
using namespace std;

static SBAudioEngine *instance = nullptr;
SBAudioEngine* SBAudioEngine::getInstance() {
    
    if( !instance ) {
        instance = new SBAudioEngine();
    }
    
    return instance;
}

void SBAudioEngine::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

SBAudioEngine::SBAudioEngine() :
mute(false),
paused(false),
bgm("") {
}

SBAudioEngine::~SBAudioEngine() {
}

/**
 * 매니저 초기화
 */
void SBAudioEngine::init() {
    
    setMute(UserDefault::getInstance()->getBoolForKey(SBUserDefaultKey::MUTE));
}

/**
 * mute 설정
 */
void SBAudioEngine::setMute(bool mute) {
    
    this->mute = mute;
    
    // update user default
    UserDefault::getInstance()->setBoolForKey(SBUserDefaultKey::MUTE, mute);
    UserDefault::getInstance()->flush();
    
    // update audio engine
    const float VOLUME = getVolume();
    
    for( auto it = _audioIDInfoMap.begin(); it != _audioIDInfoMap.end(); ++it ) {
        AudioEngine::setVolume(it->first, VOLUME);
//        _audioEngineImpl->setVolume((int)it->first, VOLUME);
//        it->second.volume = VOLUME;
    }
}

/**
 * play
 */
int SBAudioEngine::play2d(const string &file, bool loop) {
    
    auto audioId = AudioEngine::play2d(file, loop, getVolume());
    
    if( instance->paused ) {
        // TODO:
    }
    
    return audioId;
}

/**
 * resume
 */
void SBAudioEngine::resumeAll() {
    
    instance->paused = false;
    AudioEngine::resumeAll();
}

void SBAudioEngine::resume(const string &file) {
    
    auto audioIds = getAudioId(file);
    
    for( int audioId : audioIds ) {
        AudioEngine::resume(audioId);
    }
}

/**
 * pause
 */
void SBAudioEngine::pauseAll() {
    
    instance->paused = true;
    AudioEngine::pauseAll();
}

void SBAudioEngine::pause(const string &file) {
    
    auto audioIds = getAudioId(file);
    
    for( int audioId : audioIds ) {
        AudioEngine::pause(audioId);
    }
}

/**
 * stop
 */
void SBAudioEngine::stop(const string &file) {
    
    auto audioIds = getAudioId(file);
    
    for( int audioId : audioIds ) {
        AudioEngine::stop(audioId);
    }
}

void SBAudioEngine::setOnFinishedListener(int audioId, const function<void(int, const string&)> &listener) {
    
    AudioEngine::setFinishCallback(audioId, listener);
}

float SBAudioEngine::getDuration(int audioId) {
    
    float duration = AudioEngine::getDuration(audioId);
    return duration;
}

float SBAudioEngine::getDuration(const string &file) {
    
    auto audioIds = getAudioId(file);
    if( audioIds.size() == 0 ) {
        return 0;
    }
    
    float duration = AudioEngine::getDuration(*audioIds.begin());
    instance->durationMap[file] = duration;
    
    return duration;
}

/**
 * 배경음 재생
 */
int SBAudioEngine::playBGM(const string &bgm, bool loop) {
    
    if( bgm == "" ) {
        return AudioEngine::INVALID_AUDIO_ID;
    }
    
    if( instance->bgm == bgm ) {
        return *getAudioId(bgm).begin();
    }
    
    stopBGM(); // 이전 BGM 정지
    
    instance->bgm = bgm;
    return SBAudioEngine::play2d(bgm, loop);
}

/**
 * 스테이지 배경음 재게
 */
void SBAudioEngine::resumeBGM() {
    
    if( instance->bgm != "" ) {
        resume(instance->bgm);
    }
}

/**
 * 스테이지 배경음 일시정지
 */
void SBAudioEngine::pauseBGM() {
    
    if( instance->bgm != "" ) {
        pause(instance->bgm);
    }
}

/**
 * 스테이지 배경음 정지
 */
void SBAudioEngine::stopBGM() {
    
    if( instance->bgm != "" ) {
        stop(instance->bgm);
        instance->bgm = "";
    }
}

list<int> SBAudioEngine::getAudioId(const string &file) {
    
    auto it = _audioPathIDMap.find(file);
    if( it != _audioPathIDMap.end() ) {
        return it->second;
    }
    
    return list<int>();
}

float SBAudioEngine::getVolume() {
    
    return instance->mute ? 0 : 1;
}


