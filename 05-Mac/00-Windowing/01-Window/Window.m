#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to windows.h

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>  //next step - NS
@end

int main(int argc, char* argv[])
{
    //code
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc]init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate : [[AppDelegate alloc]init]];
    
    [NSApp run];       //this is called as run loop / message loop / event loop
    
    [pool release];
    
    return(0);
}

@interface MyView : NSView
@end

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

@implementation MyView
{
    @private
    NSString *centralText;
}

-(id)initWithFrame : (NSRect)frame
{
    //code
    self = [super initWithFrame : frame];
    
    if(self)
    {
        centralText = @"Hello World!";
    }
    
    return(self);
}

-(void)drawRect : (NSRect)dirtyRect
{
    //code
    NSColor *backgroundColor = [NSColor blackColor];
    [backgroundColor set];
    NSRectFill(dirtyRect);
    
    NSDictionary *dictionaryForTextAttributes = [NSDictionary dictionaryWithObjectsAndKeys : [NSFont fontWithName : @"Helvetica" size : 32], NSFontAttributeName,
                                                                                             [NSColor greenColor], NSForegroundColorAttributeName, nil];
    
    NSSize textSize = [centralText sizeWithAttributes : dictionaryForTextAttributes];
    
    NSPoint point;
    point.x = (dirtyRect.size.width/2) - (textSize.width/2);
    point.y = (dirtyRect.size.height/2) - (textSize.height/2) + 12;
    
    [centralText drawAtPoint : point withAttributes : dictionaryForTextAttributes];
    
}

-(BOOL)acceptsFirstResponder
{
    //code
    [[self window]makeFirstResponder : self];
    
    return(YES);
}

-(void)keyDown : (NSEvent *)theEvent
{
    //code
    int key = [[theEvent characters]characterAtIndex : 0];
    
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate : self];
            break;
            
        case 'F':
        case 'f':
            [[self window]toggleFullScreen : self];
            break;
    }
}

-(void)mouseDown : (NSEvent *)theEvent
{
    //code
    centralText = @"Left Mouse Button Clicked";
    [self setNeedsDisplay : YES];
}

-(void)rightMouseDown : (NSEvent *)theEvent
{
    //code
    centralText = @"Right Mouse Button Clicked";
    [self setNeedsDisplay : YES];
}

-(void)otherMouseDown : (NSEvent *)theEvent
{
    //code
    centralText = @"Hello World!";
    [self setNeedsDisplay : YES];
}

-(void)dealloc
{
    //code
    [super dealloc];
}

@end


