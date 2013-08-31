//
//  ofxQTKitAVScreenRecorder.h
//  ofxQTKitAVScreenRecorder
//
//  Created by Seth Hunter (hunters@mit.edu) on 8/14/12. 
//  2012 MIT Media Lab - use it as you please. 
//

#pragma once  

#ifndef QTKIT_FBO_AUDIO_RECORDER_H  
#define QTKIT_FBO_AUDIO_RECORDER_H 

#include "ofxQTKitVideoGrabber.h"
#include "ofxQTKitRecorder.h"


class ofxQTKitAVScreenRecorder {  
public: 
    ofxQTKitAVScreenRecorder();  
    ~ofxQTKitAVScreenRecorder(); 
    void setup(); 
    void update(); 
    void exit(); 
    void initAudio(); 
    void initAVRecording();
    void startNewRecording(); 
    void finishMovie(bool exiting=false); 
    void addFrame(ofPixels& framePixels); 
    bool getIsRecording() { return isRecording; }
    
    string audioFullPath;
    string videoFullPath;
    string selfContainedFullPath; 
    
	ofxQTKitRecorder	 recorder;
    ofxQTKitVideoGrabber audioTrack; 

private: 
    int addAudioTrack; 
    bool stopAddingFrames;
    bool isRecording; 
    void doWeNeedToAddAudio(); 

};  


#endif