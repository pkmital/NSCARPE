#import "AppDelegate.h"

@implementation AppDelegate

- (void) applicationDidFinishLaunching: (NSNotification*) notification {
    bAllocated = false;
}

- (void) applicationWillTerminate: (NSNotification*) notification {
    if(bAllocated)
    {
        delete appPtr;
        bAllocated = false;
    }
	ofxNSWindower::destroy();
}

- (IBAction)loadStudy:(id)sender {
    if(bAllocated)
    {
        delete appPtr;
        bAllocated = false;
    }
    appPtr = new testApp();
	ofxNSWindower::instance()->addWindow(appPtr, "C.A.R.P.E.: Stimulus Display", NSTitledWindowMask, 0);
    
    bAllocated = true;
}
@end
