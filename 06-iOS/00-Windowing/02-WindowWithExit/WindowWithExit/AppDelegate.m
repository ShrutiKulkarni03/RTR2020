//
//  AppDelegate.m
//  Window
//
//  Created by Shruti Kulkarni on 03/07/21.
//

#import "AppDelegate.h"

#import "ViewController.h"

#import "MyView.h"


@implementation AppDelegate
{
    @private
    UIWindow *window;
    ViewController *viewController;
    MyView *view;
    
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Override point for customization after application launch.
    CGRect win_rect = [[UIScreen mainScreen]bounds];
    
    window = [[UIWindow alloc]initWithFrame:win_rect];
    
    viewController = [[ViewController alloc]init];
    
    [window setRootViewController:viewController];
    
    view = [[MyView alloc]initWithFrame:win_rect];
    
    [viewController setView:view];
    [view release];
    
    [window makeKeyAndVisible];
    
    return(YES);
}

-(void)applicationWillResignActive:(UIApplication *)application
{
    //code
}

-(void)applicationDidEnterBackground:(UIApplication *)application
{
    //code
}

-(void)applicationWillEnterForeground:(UIApplication *)application
{
    //code
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
    //code
}

-(void)applicationWillTerminate:(UIApplication *)application
{
    //code
}

-(void)dealloc
{
    //code
    [view release];
    [viewController release];
    [window release];
    [super dealloc];
    
}

@end
