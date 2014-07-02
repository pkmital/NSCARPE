#import "AppDelegate.h"

@implementation AppDelegate
@synthesize OpticalFlow;
@synthesize exportMotionDescriptorsToHDF5Item;
@synthesize toggleHeatmap;
@synthesize toggleClustering;

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

- (IBAction)exportMotionDescriptorsToHDF5:(id)sender {
    appPtr->exportMotionDescriptorsToHDF5();

}
- (IBAction)toggleEyes:(id)sender {
    appPtr->toggleEyes();
}

- (IBAction)toggleSaccades:(id)sender {
    appPtr->toggleSaccades();
}

- (IBAction)toggleHeatmap:(id)sender {
    appPtr->toggleHeatmap();
}

- (IBAction)toggleDifferenceHeatmap:(id)sender {
    appPtr->toggleDifferenceHeatmap();
}

- (IBAction)toggleClustering:(id)sender {
    appPtr->toggleClustering();
}

- (IBAction)loadStudy:(id)sender {
    if(bAllocated)
    {
        delete appPtr;
        bAllocated = false;
    }
    appPtr = new testApp();
	ofxNSWindower::instance()->addWindow(appPtr, "C.A.R.P.E.: Stimulus Display", NSTitledWindowMask, 0);
    
//    [exportMotionDescriptorsToHDF5Item setEnabled:true];
    
    bAllocated = true;
}
@end
