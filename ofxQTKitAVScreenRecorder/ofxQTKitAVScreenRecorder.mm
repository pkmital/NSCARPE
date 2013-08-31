//
//  ofxQTKitAVScreenRecorder.m
//  ofxQTKitAVScreenRecorder
//
//  Created by Seth Hunter (hunters@mit.edu) on 8/14/12. 
//  2012 MIT Media Lab - use it as you please. 
//

#import "ofxQTKitAVScreenRecorder.h"


ofxQTKitAVScreenRecorder::ofxQTKitAVScreenRecorder() {  
    stopAddingFrames = false; 
    isRecording = false;
    addAudioTrack = 0; 

}  

ofxQTKitAVScreenRecorder::~ofxQTKitAVScreenRecorder() {  
    
}  

void ofxQTKitAVScreenRecorder::setup() {
    initAudio(); 
}

void ofxQTKitAVScreenRecorder::initAudio() {
    //initialize the audio source
    audioTrack.initAudioGrabber();
    audioTrack.setAudioDeviceID("Built-in Microphone");
    audioTrack.setAudioCodec("QTCompressionOptionsVoiceQualityAACAudio");  //maybe choose a different codec? This one seems rather poor. 
    audioTrack.initAudioRecording(); 
}


void ofxQTKitAVScreenRecorder::initAVRecording() {
    
    //temp audio file
    char $audiofilename[1024];
	time_t now = time(NULL);
	struct tm *ts = localtime(&now);
	strftime($audiofilename, sizeof($audiofilename), "%m_%d_%H_%M_%S_audio.mov", ts);
    audioFullPath = ofToDataPath(string($audiofilename));    
    audioTrack.startRecording(audioFullPath);
    
    //temp video file
    char $videofilename[1024];
	now = time(NULL);
	struct tm *ts2 = localtime(&now);
	strftime($videofilename, sizeof($videofilename), "%m_%d_%H_%M_%S_video.mov", ts2);
    videoFullPath = ofToDataPath(string($videofilename));
    recorder.setup(videoFullPath);
    
    //final output combining both files. 
    char $temp[1024];
	now = time(NULL);
	struct tm *ts3 = localtime(&now);
	strftime($temp, sizeof($temp), "%m_%d_%H_%M_%S_telekinectRecording.mov", ts3);
    selfContainedFullPath = ofToDataPath(string($temp));
    
    isRecording = true; 
}

void ofxQTKitAVScreenRecorder::startNewRecording() {
    if(!isRecording) {
        recorder.releaseRecording();
        initAVRecording();
        stopAddingFrames = false;
    } else {
        printf("/n AUDIO RECORDER: a recording is already in progress "); 
    }
}

void ofxQTKitAVScreenRecorder::update() {
    doWeNeedToAddAudio();
}

void ofxQTKitAVScreenRecorder::addFrame(ofPixels& framePixels) {
    if(!stopAddingFrames) recorder.addFrame(framePixels);
}

void ofxQTKitAVScreenRecorder::exit() {
    if(!stopAddingFrames) {
        finishMovie(true); 
    }
}

//3 second process is safe to output most files in my testing (I couldn't get event handlers working)
void ofxQTKitAVScreenRecorder::doWeNeedToAddAudio() {
    if(addAudioTrack > 0) {
        addAudioTrack++; 
        //delay adding the audio track by 1 second so that the threaded QTKit can load the flushed audio
        if(addAudioTrack == 24) {
            recorder.loadAudioTrack((char*)audioFullPath.c_str());
        }
        //wait one more second and append the audio track to the video track and save as a new movie. 
        else if(addAudioTrack == 48) {
            recorder.addAudioTrack((char*)selfContainedFullPath.c_str());
        }
        //one last second then remove the temp files. 
        else if(addAudioTrack == 72) {
            recorder.removeTempFiles((char*)audioFullPath.c_str(), (char*)videoFullPath.c_str());
            addAudioTrack=0;
            isRecording = false;
            printf("\n AV Recorder: File has been recorded sucessfully as: %s", selfContainedFullPath.c_str());
        }
    }
}


void ofxQTKitAVScreenRecorder::finishMovie(bool exiting) {
    if(isRecording) {
        audioTrack.stopRecording(); 
        recorder.flushToFile();
        if(exiting) {
            ofSleepMillis(1000);
            recorder.addAudioTrack((char*)videoFullPath.c_str());
        } else {
            stopAddingFrames = true;  //toggle prevents the movie from accumulating anymore. 
            addAudioTrack = 1;
        }
    } else {
        printf("\nAUDIO RECORDER: No recording is available to finish.");
    }
}
