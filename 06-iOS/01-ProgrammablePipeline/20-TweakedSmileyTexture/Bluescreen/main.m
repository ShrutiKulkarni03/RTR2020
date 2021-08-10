//
//  main.m
//  Window
//
//  Created by Shruti Kulkarni on 03/07/21.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[])
{
    //code
    NSString *appDelegateClassName;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    
    appDelegateClassName = NSStringFromClass([AppDelegate class]);
    
    int ret = UIApplicationMain(argc, argv, nil, appDelegateClassName);
    
    [pool release];
   
    return(ret);
}
