#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to windows.h
#import "MyView.h"
#import "AppDelegate.h"

@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyView *view;
}

-(void)applicationDidFinishLaunching : (NSNotification *)aNotification
{
    //code
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    
    window = [[NSWindow alloc]initWithContentRect : win_rect
                                        styleMask : NSWindowStyleMaskTitled |
                                                    NSWindowStyleMaskClosable |
                                                    NSWindowStyleMaskMiniaturizable |
                                                    NSWindowStyleMaskResizable
                                          backing : NSBackingStoreBuffered
                                           defer : NO];
    
    [window setTitle : @"SPK : MacOS Window"];
    [window center];
    
    view = [[MyView alloc]initWithFrame : win_rect];
    
    [window setContentView : view];
    [window setDelegate : self];
    [window makeKeyAndOrderFront : self];
    
}

-(void)applicationWillTerminate : (NSNotification *)aNotification
{
    //code
    
}

-(void)windowWillClose : (NSNotification *)aNotification
{
    //code
    [NSApp terminate : self];
}

-(void)dealloc
{
    //code
    [view release];
    [window release];
    [super dealloc];
    
}

@end
