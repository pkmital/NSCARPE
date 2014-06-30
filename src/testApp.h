
#pragma once

#include "ofMain.h"
#include "ofxNSWindowApp.h"
#include "ofxXmlSettings.h"
#include "ofxQTKitRecorder.h"
#include "pkmEyeMovementCollection.h"
#include "pkmOpticalFlow.h"


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
    void initializeMovie(string movieURL);
    void initializeRecording(string saveURL);
    void initializeEyeTrackingData(vector<string> paths);
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
	
private:
    //--------------------------------------------------------------
    // Movie 
    ofQTKitPlayer                   moviePlayer;
    string                          movieURL, movieName;
    float                           movieFrameNumber, movieTotalFrames, movieMSPerFrame, frameIncrement;
    int                             movieWidth, movieHeight;
    float                           movieFrameRate;
    int                             movieOffsetX, movieOffsetY;                 // offset of movie within monitor
    
    
    //--------------------------------------------------------------
    // Recording 
    ofxQTKitRecorder                recorder;
    bool                            bDoneRecording;
    ofFbo                           recorderFbo;
    
    
    //--------------------------------------------------------------
    // Optical Flow Map
    pkmOpticalFlow                  flow;
    
    
    //--------------------------------------------------------------
    // Eye-Data
    pkmEyeMovementCollection        eyeMovements;
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
    // Control variables
    bool                            bLoadBinocular, 
                                    bLoadClassifiedData,
                                    bShowRealTime,
                                    bShowClustering,
                                    bShowAlphaScreen,
                                    bShowContours,
                                    bShowEyes,
                                    bShowHeatmap,
                                    bShowMeanBinocular,
                                    bShowMovie,
                                    bShowFlow,
                                    bSaveMovie,
                                    bSaveMovieImages,
                                    bShowNormalized,
                                    bShowRadialBlur,
                                    bShowHistogram,
                                    bShowSaccades,
                                    bShowSubjectNames,
                                    bCalculateMovieOffset,
                                    bLoadArringtonResearchFormat,
                                    bSetup;
    //--------------------------------------------------------------
	
};
//--------------------------------------------------------------