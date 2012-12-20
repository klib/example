//
//  AppDelegate.m
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#import "AppDelegate.h"
#import	"KLib.h"
#import "KLibDelegate.h"
#import "ViewController.h"

@implementation AppDelegate

//===========================================================
/**
 *	@brief	解放処理
 */
//===========================================================
- (void)dealloc
{
	[_window release];
	[_viewController release];
    [super dealloc];
}


//===========================================================
/**
 *	@brief	アプリが起動した
 */
//===========================================================
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
	NSSetUncaughtExceptionHandler(KL_uncaughtExceptionHandler);
	_viewController = [[ViewController alloc] init];
	self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    return YES;
}

//===========================================================
/**
 *	@brief	非アクティブになる時
 */
//===========================================================
- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}


//===========================================================
/**
 *	@brief	非アクティブになった時
 */
//===========================================================
- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	KL_DidEnterBackground();
	
}


//===========================================================
/**
 *	@brief	アクティブになる時
 */
//===========================================================
- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	KL_WillEnterForeground();
}

//===========================================================
/**
 *	@brief	アクティブになった
 */
//===========================================================
- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

//===========================================================
/**
 *	@brief	終了する時
 */
//===========================================================
- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
