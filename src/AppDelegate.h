#include "testApp.h"
#include "ofxNSWindower.h"
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    testApp *appPtr;
    bool bAllocated;
}

- (IBAction)loadStudy:(id)sender;
@end
