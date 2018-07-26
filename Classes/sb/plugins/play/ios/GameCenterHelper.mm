//
//  GameCenterHelper.m
//
//  Created by seongmin hwang on 2018. 7. 22..
//

#import "GameCenterHelper.h"

#import "AppController.h"
#import "RootViewController.h"

#include "../../../platform/SBPlatformMacros.h"

USING_NS_CC;
using namespace std;

@interface GameCenterHelper (Private)

- (id) init;
- (void) authenticateLocalPlayer;

@end

@implementation GameCenterHelper

+ (GameCenterHelper*) getInstance {
    
    static GameCenterHelper *instance = nil;
    static dispatch_once_t oncePredicate;
    
    dispatch_once(&oncePredicate, ^{
        instance = [[GameCenterHelper alloc] init];
    });
    
    return instance;
}

- (id) init {
    
    if( self = [super init] ) {
    }
    
    return self;
}

/**
 * 플레이어 인증
 */
- (void) authenticateLocalPlayer {
    
    NSLog(@"authenticateLocalPlayer isAuthenticated: %d", [GameCenterHelper isSignedIn]);
    
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    localPlayer.authenticateHandler = ^(UIViewController *loginViewController, NSError *error){
    
        NSLog(@"authenticateLocalPlayer completion viewController: %d isAuthenticated: %d error: %@",
              loginViewController != nullptr,
              [GameCenterHelper isSignedIn],
              (error != nil) ? error : @"");
        
        // show game center
        if( loginViewController != nil ) {
            [ROOT_VIEW_CONTROLLER presentViewController:loginViewController animated:YES completion:^{
                NSLog(@"Finished Game Center.");
            }];
        }
        // player was authenticated
        else if( localPlayer.isAuthenticated ) {
        }
        // Game Center should be disabled
        else {
            
        }
    };
}

+ (void) signIn {
    
    if( [GameCenterHelper isSignedIn] ) {
        return;
    }
    
    [[GameCenterHelper getInstance] authenticateLocalPlayer];
}

+ (void) signOut {
    
    if( ![GameCenterHelper isSignedIn] ) {
        return;
    }
}

+ (BOOL) isSignedIn {
    return [GKLocalPlayer localPlayer].isAuthenticated;
}

+ (NSString*) getPlayerId {
    return [GKLocalPlayer localPlayer].playerID;
}

+ (void) showAllLeaderboards {
    
    GKGameCenterViewController *controller = [[GKGameCenterViewController alloc] init];
    controller.gameCenterDelegate = [GameCenterHelper getInstance];
    controller.viewState = GKGameCenterViewControllerStateLeaderboards;
    
    [ROOT_VIEW_CONTROLLER presentViewController:controller animated:YES completion:^{
        NSLog(@"showAllLeaderboards completion");
    }];
}

+ (void) showLeaderboard:(NSString *)leaderboardName {

    GKGameCenterViewController *controller = [[GKGameCenterViewController alloc] init];
    controller.gameCenterDelegate = [GameCenterHelper getInstance];
    controller.viewState = GKGameCenterViewControllerStateLeaderboards;
    controller.leaderboardIdentifier = leaderboardName;
    
    [ROOT_VIEW_CONTROLLER presentViewController:controller animated:YES completion:^{
        NSLog(@"showLeaderboard completion");
    }];
}

+ (void) submitScore:(NSString *)leaderboardName :(long)score {
 
    // int64_t score = (int64_t)highScore;
    GKScore *scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier:leaderboardName];
    scoreReporter.value = score;
    scoreReporter.context = 0;
    
    NSArray *scores = @[scoreReporter];
    [GKScore reportScores:scores withCompletionHandler:^(NSError *error) {
        //Do something interesting here.
    }];
}

+ (void) showAchievements {
    
    GKGameCenterViewController *controller = [[GKGameCenterViewController alloc] init];
    controller.gameCenterDelegate = [GameCenterHelper getInstance];
    controller.viewState = GKGameCenterViewControllerStateAchievements;
    
    [ROOT_VIEW_CONTROLLER presentViewController:controller animated:YES completion:^{
        NSLog(@"showAchievements completion");
    }];
}

+ (void) loadAchievements:(BOOL)forceReload {
    
    // TODO:
}

+ (void) reportAchievement:(NSString *)achievementName :(float)percent {

    GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier:achievementName];
    achievement.showsCompletionBanner = YES;
    achievement.percentComplete = percent;
    
    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError *error) {
        if( error != nil ) {
            NSLog(@"Error in reporting achievements: %@", error);
        }
    }];
}

+ (void) unlockAchievement:(NSString *)achievementName {
    
    [GameCenterHelper reportAchievement:achievementName :100];
}

+ (void) incrementAchievement:(NSString *)achievementName :(int)increment {
 
    // TODO:
}

+ (void) reveal:(NSString *)achievementName {
    
    // TODO:
}

+ (void) setSteps:(NSString *)achievementName :(double)steps {
    
    // TODO:
}

+ (void) resetAchievements {
    
    // TODO:
}

///////////////////////////////////////////////////
//////////// GKGameCenterControllerDelegate
///////////////////////////////////////////////////

- (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)controller {
    
    [controller dismissViewControllerAnimated:YES completion:nil];
}

@end


