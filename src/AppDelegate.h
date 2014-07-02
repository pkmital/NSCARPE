#include "testApp.h"
#include "ofxNSWindower.h"
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    testApp *appPtr;
    bool bAllocated;
    NSMenuItem *exportMotionDescriptorsToHDF5Item;
    NSMenuItem *toggleHeatmap;
    NSMenuItem *toggleClustering;
    NSMenuItem *OpticalFlow;
}

// FILE
@property (assign) IBOutlet NSMenuItem *exportMotionDescriptorsToHDF5Item;

// VIEW
@property (assign) IBOutlet NSMenuItem *toggleHeatmap;
@property (assign) IBOutlet NSMenuItem *toggleClustering;
@property (assign) IBOutlet NSMenuItem *OpticalFlow;

// FILE
- (IBAction)loadStudy:(id)sender;
- (IBAction)exportMotionDescriptorsToHDF5:(id)sender;

// VIEW
- (IBAction)toggleHeatmap:(id)sender;
- (IBAction)toggleClustering:(id)sender;

@end
