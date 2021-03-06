/*
 
 © Parag K Mital, parag@pkmital.com
 
 The Software is and remains the property of Parag K Mital
 ("pkmital") The Licensee will ensure that the Copyright Notice set
 out above appears prominently wherever the Software is used.
 
 The Software is distributed under this Licence:
 
 - on a non-exclusive basis,
 
 - solely for non-commercial use in the hope that it will be useful,
 
 - "AS-IS" and in order for the benefit of its educational and research
 purposes, pkmital makes clear that no condition is made or to be
 implied, nor is any representation or warranty given or to be
 implied, as to (i) the quality, accuracy or reliability of the
 Software; (ii) the suitability of the Software for any particular
 use or for use under any specific conditions; and (iii) whether use
 of the Software will infringe third-party rights.
 
 pkmital disclaims:
 
 - all responsibility for the use which is made of the Software; and
 
 - any liability for the outcomes arising from using the Software.
 
 The Licensee may make public, results or data obtained from, dependent
 on or arising out of the use of the Software provided that any such
 publication includes a prominent statement identifying the Software as
 the source of the results or the data, including the Copyright Notice
 and stating that the Software has been made available for use by the
 Licensee under licence from pkmital and the Licensee provides a copy of
 any such publication to pkmital.
 
 The Licensee agrees to indemnify pkmital and hold them
 harmless from and against any and all claims, damages and liabilities
 asserted by third parties (including claims for negligence) which
 arise directly or indirectly from the use of the Software or any
 derivative of it or the sale of any products based on the
 Software. The Licensee undertakes to make no liability claim against
 any employee, student, agent or appointee of pkmital, in connection
 with this Licence or the Software.
 
 
 No part of the Software may be reproduced, modified, transmitted or
 transferred in any form or by any means, electronic or mechanical,
 without the express permission of pkmital. pkmital's permission is not
 required if the said reproduction, modification, transmission or
 transference is done without financial return, the conditions of this
 Licence are imposed upon the receiver of the product, and all original
 and amended source code is included in any transmitted product. You
 may be held legally responsible for any copyright infringement that is
 caused or encouraged by your failure to abide by these terms and
 conditions.
 
 You are not permitted under this Licence to use this Software
 commercially. Use for which any financial return is received shall be
 defined as commercial use, and includes (1) integration of all or part
 of the source code or the Software into a product for sale or license
 by or on behalf of Licensee to third parties or (2) use of the
 Software or any derivative of it for research with the final aim of
 developing software products for sale or license to a third party or
 (3) use of the Software or any derivative of it for research with the
 final aim of developing non-software products for sale or license to a
 third party, or (4) use of the Software to provide any service to an
 external organisation for which payment is received. If you are
 interested in using the Software commercially, please contact pkmital to
 negotiate a licence. Contact details are: parag@pkmital.com
 
 */


#include "testApp.h"
#include "pkmAudioWaveform.h"
#include "ofxNSWindower.h"
#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    testApp *appPtr;
    pkmTimelineApp *timelinePtr;
    bool bAllocated;
    
    NSMenuItem *exportMotionDescriptorsToHDF5Item;
    NSMenuItem *exportFrame;
    
    NSMenuItem *viewMenu;
    NSMenuItem *viewMenu_Movie;
    NSMenuItem *viewMenu_Eyes;
    NSMenuItem *viewMenu_Saccades;
    NSMenuItem *viewMenu_Heatmap;
    NSMenuItem *viewMenu_DifferenceHeatmap;
    NSMenuItem *viewMenu_Clustering;
    NSMenuItem *viewMenu_MotionMagnitude;
    NSMenuItem *viewMenu_MotionDirection;
    NSMenuItem *viewMenu_Normalization;
    NSMenuItem *playbackMenu;
    NSMenuItem *playbackMenu_RealTimePlayback;
    NSMenuItem *openStudyMenuItem;
}

// FILE ITEMS:
@property (assign) IBOutlet NSMenuItem *openStudyMenuItem;
@property (assign) IBOutlet NSMenuItem *exportMotionDescriptorsToHDF5Item;
@property (assign) IBOutlet NSMenuItem *exportFrame;

// FILE ACTIONS:
- (IBAction)loadStudy:(id)sender;
- (IBAction)exportMotionDescriptorsToHDF5:(id)sender;

// VIEW ITEMS:
@property (assign) IBOutlet NSMenuItem *viewMenu;
@property (assign) IBOutlet NSMenuItem *viewMenu_Movie;
@property (assign) IBOutlet NSMenuItem *viewMenu_Eyes;
@property (assign) IBOutlet NSMenuItem *viewMenu_Saccades;
@property (assign) IBOutlet NSMenuItem *viewMenu_Heatmap;
@property (assign) IBOutlet NSMenuItem *viewMenu_DifferenceHeatmap;
@property (assign) IBOutlet NSMenuItem *viewMenu_Clustering;
@property (assign) IBOutlet NSMenuItem *viewMenu_MotionMagnitude;
@property (assign) IBOutlet NSMenuItem *viewMenu_MotionDirection;
@property (assign) IBOutlet NSMenuItem *viewMenu_Normalization;

// VIEW ACTIONS:
- (IBAction)toggleHeatmap:(id)sender;
- (IBAction)toggleClustering:(id)sender;

// PLAYBACK ITEMS:
@property (assign) IBOutlet NSMenuItem *playbackMenu;
@property (assign) IBOutlet NSMenuItem *playbackMenu_RealTimePlayback;

// PLAYBACK ACTIONS:



@end
