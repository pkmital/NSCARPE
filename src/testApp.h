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


#pragma once

#include "ofMain.h"
#include "ofxNSWindowApp.h"
#include "ofxXmlSettings.h"
#include "ofxQTKitRecorder.h"
#include "pkmEyeMovementCollection.h"
#include "pkmOpticalFlow.h"
#include "pkmAudioWindow.h"
#include "pkmAudioWaveform.h"


//--------------------------------------------------------------
class testApp : public ofxNSWindowApp {

public:
    //--------------------------------------------------------------
	testApp() {}
    ~testApp() { close(); }
	void close();
    //--------------------------------------------------------------
	
    //--------------------------------------------------------------
    void setup();
    void update();
    void draw();
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void initializeVisualSaliency();
    void initializeMovie();
    void initializeRecording(string saveURL);
    void initializeEyeTrackingData(vector<string> paths);
    void initializeExperiment();
    void initializeTimeline();
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void loadUserSettings();
    void saveUserSettings();
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
	void keyPressed(int key);
	void keyReleased(int key);
    //--------------------------------------------------------------
	
    //--------------------------------------------------------------
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(float x, float y);
    //--------------------------------------------------------------

    //--------------------------------------------------------------
    void exportMotionDescriptorsToHDF5();
    void exportFrame();
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleHeatmap() {
        bShowHeatmap = !bShowHeatmap; return bShowHeatmap;
    }
    bool isHeatmapEnabled() {
        return bShowHeatmap;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleClustering() {
        bShowClustering = !bShowClustering; return bShowClustering;
    }
    bool isClusteringEnabled() {
        return bShowClustering;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleEyes() {
        bShowEyes = !bShowEyes; return bShowEyes;
    }
    bool isEyesEnabled() {
        return bShowEyes;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleSaccades() {
        bShowSaccades = !bShowSaccades; return bShowSaccades;
    }
    bool isSaccadesEnabled() {
        return bShowSaccades;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleDifferenceHeatmap() {
        bShowDifferenceHeatmap = !bShowDifferenceHeatmap; return bShowDifferenceHeatmap;
    }
    bool isDifferenceHeatmapEnabled() {
        return bShowDifferenceHeatmap;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleNormalization() {
        bShowNormalized = !bShowNormalized; return bShowNormalized;
    }
    bool isNormalizationEnabled() {
        return bShowNormalized;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleFlow() {
        bShowFlow = !bShowFlow; return bShowFlow;
    }
    bool isFlowEnabled() {
        return bShowFlow;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleFlowMagnitude() {
        bShowFlowMagnitude = !bShowFlowMagnitude;
        if(bShowFlowMagnitude)
            bShowFlow = true;
        else if(!bShowFlowDirection && !bShowFlowMagnitude && !bExportMotionDescriptorsToHDF)
            bShowFlow = false;
        return bShowFlowMagnitude;
    }
    bool isFlowMagnitudeEnabled() {
        return bShowFlowMagnitude;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleFlowDirection() {
        bShowFlowDirection = !bShowFlowDirection;
        if(bShowFlowDirection)
            bShowFlow = true;
        else if(!bShowFlowDirection && !bShowFlowMagnitude && !bExportMotionDescriptorsToHDF)
            bShowFlow = false;
        return bShowFlowDirection;
    }
    bool isFlowDirectionEnabled() {
        return bShowFlowDirection;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool toggleRealTimePlayback() {
        bShowRealTime = !bShowRealTime;
        if(bShowRealTime) {
            if(moviePlayer != NULL) {
                moviePlayer->play();
            }
        }
        else {
            if(moviePlayer != NULL) {
                moviePlayer->stop();
            }
        }
        return bShowRealTime;
    }
    bool isRealTimePlayback() {
        return bShowRealTime;
    }
    //--------------------------------------------------------------
    
    
    //--------------------------------------------------------------
    bool toggleMovie() {
        bShowMovie = !bShowMovie; return bShowMovie;
    }
    bool isMovieEnabled() {
        return bShowMovie;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    string getAudioURL() {
        return audioURL;
    }
    string getMovieURL() {
        return movieURL;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setTimelinePtr(pkmTimelineApp* ptr) {
        timelinePtr = ptr;
    }
    
    //--------------------------------------------------------------
    int getWidth() {
        return movieWidth;
    }
    int getHeight() {
        return movieHeight;
    }
    //--------------------------------------------------------------
    
    
    //--------------------------------------------------------------
    bool shouldVisualizeAudio() {
        return (bPlayAudio || bSaveAudio);
    }
    bool shouldPlayAudio() {
        return bPlayAudio;
    }
    //--------------------------------------------------------------
	
private:
    //--------------------------------------------------------------
    // Controller
    pkmTimelineApp                  *timelinePtr;
    
    //--------------------------------------------------------------
    // Movie 
    ofQTKitPlayer                   *moviePlayer;
    string                          movieURL, movieName, audioURL;
    float                           movieFrameNumber, movieTotalFrames, movieMSPerFrame, frameIncrement;
    int                             movieWidth, movieHeight;
    float                           movieFrameRate;
    int                             movieOffsetX, movieOffsetY;                 // offset of movie within monitor
    
    
    //--------------------------------------------------------------
    // Recording 
    ofxQTKitRecorder                recorder;
    bool                            bDoneRecording;
    ofFbo                           recorderFbo;
    string                          saveMovieURL, saveFilename;
    
    
    //--------------------------------------------------------------
    // Optical Flow Map
    pkmOpticalFlow                  flow;
    
    
    //--------------------------------------------------------------
    // Eye-Data
    pkmEyeMovementCollection        *eyeMovements;
    float                           eyeMovementFrameRate;
    
    
    //--------------------------------------------------------------
    // Screen 
    int                             screenWidth, screenHeight;                  // resolution of monitor when movie was displayed
    
    
    //--------------------------------------------------------------
	// readback
	GLuint                          *pboIds;				// IDs of PBOs
    ofPixels                        readbackPixels;
    unsigned char                   *readbackPixelsBuffer;
    int                             asyncReadingIndex, asyncCopyingIndex;
    
    
    //--------------------------------------------------------------
	// Experiment info
    ofxXmlSettings                  settings;
    int                             numberOfExperiments;
    int                             currentExperiment;
    int                             heatmapType;
    
    
    //--------------------------------------------------------------
    // Control variables
    bool                            bLoadBinocular,
                                    bLoadMillisecondFormat,
                                    bLoadClassifiedData,
                                    bShowRealTime,
                                    bShowClustering,
                                    bShowAlphaScreen,
                                    bShowContours,
                                    bShowEyes,
                                    bShowSaccades,
                                    bShowHeatmap,
                                    bShowDifferenceHeatmap,
                                    bShowMeanBinocular,
                                    bShowMovie,
                                    bPlayAudio,
                                    bSaveAudio,
                                    bShowFlow,
                                    bShowFlowMagnitude,
                                    bShowFlowDirection,
                                    bSaveMovie,
                                    bSaveMovieImages,
                                    bShowNormalized,
                                    bShowRadialBlur,
                                    bShowHistogram,
                                    bShowSubjectNames,
                                    bCalculateMovieOffset,
                                    bLoadArringtonResearchFormat,
                                    bAutoStart,
                                    bAutoQuitAfterProcessing,
                                    bExportMotionDescriptorsToHDF,
                                    bPaused,
                                    bShowDetail,
                                    bNeedsUpdate,
                                    bSetup;
    //--------------------------------------------------------------
	
};
//--------------------------------------------------------------