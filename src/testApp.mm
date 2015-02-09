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
#include "ofxFileDialogOSX.h"
#include "ofAppRunner.h"

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a < _b ? _a : _b; })


//--------------------------------------------------------------
void testApp::setup() {

    bSetup = false;
    bPaused = false;
    bShowDetail = false;
    bNeedsUpdate = false;
    
    timelinePtr = NULL;
    
    ofSetLogLevel(OF_LOG_NOTICE);
    ofEnableArbTex();
    ofEnableSmoothing();
    ofEnableDataPath();
    
    ofSetCircleResolution(20);
}

//--------------------------------------------------------------
void testApp::close() {
    saveUserSettings();
    
    if(bSaveMovie)
    {
        recorder.flushToFile();
        
        if(bSaveAudio)
        {
            recorder.loadAudioTrack(audioURL.c_str());
            recorder.addAudioTrack(saveFilename.c_str());
            bSaveAudio = false;
        }
        
        recorder.releaseRecording();
    }
    
    if (moviePlayer != NULL) {
        moviePlayer->closeMovie();
        delete moviePlayer;
        moviePlayer = NULL;
    }
    
    if (eyeMovements != NULL)
    {
        delete eyeMovements;
        eyeMovements = NULL;
    }

}

//--------------------------------------------------------------
void testApp::update() {
    if(!bSetup)
    {
        return;
    }
    
    if (movieFrameNumber > movieTotalFrames)
    {
        ofLog(OF_LOG_NOTICE, "Reached end of movie at %f seconds.", movieFrameNumber / (float)movieFrameRate);
        
        if (moviePlayer != NULL) {
            moviePlayer->closeMovie();
            delete moviePlayer;
            moviePlayer = NULL;
        }
        
        if (eyeMovements != NULL)
        {
            delete eyeMovements;
            eyeMovements = NULL;
        }
        
        currentExperiment++;
        
        if(bSaveMovie)
        {
            recorder.flushToFile();
            
            if(bSaveAudio)
            {
                recorder.loadAudioTrack(audioURL.c_str());
                recorder.addAudioTrack(saveFilename.c_str());
                bSaveAudio = false;
            }
            
            recorder.releaseRecording();
        }
        
        if(bExportMotionDescriptorsToHDF)
        {
            exportMotionDescriptorsToHDF5();
        }
        
        if (currentExperiment < numberOfExperiments) {
            initializeExperiment();
        }
        else if (bAutoQuitAfterProcessing)
        {
            ofLog(OF_LOG_NOTICE, "Exiting...");
            close();
            OF_EXIT_APP(0);
        }
    }
    
    if (!bPaused || bNeedsUpdate)
    {
        
        if (bShowRealTime) {
            moviePlayer->update();
            movieFrameNumber = moviePlayer->getCurrentFrame();
            eyeMovements->setTime(movieFrameNumber / (float)movieFrameRate);
        }
        else {
            moviePlayer->setFrame(movieFrameNumber);
            eyeMovements->setTime(movieFrameNumber / (float)movieFrameRate);
        }
        
        if (bShowFlow) {
            flow.update(moviePlayer->getPixelsRef());
            flow.computeHistogramOfOrientedMotionGradients();
        }

        if(!bPaused)
        {
            movieFrameNumber+=frameIncrement;
            if(timelinePtr != NULL)
            {
                timelinePtr->setCurrentTime(movieFrameNumber / (float)movieFrameRate);
            }
        }
        
        ofLog(OF_LOG_NOTICE, "Processing Frame %4.0f/%4.0f", movieFrameNumber, movieTotalFrames);
        
    }
    
    bNeedsUpdate = false;
}

//--------------------------------------------------------------
void testApp::draw() {
    if(!bSetup)
    {
        return;
    }
    

    recorderFbo.begin();
    
    if (bShowMovie)
    {
        moviePlayer->draw(0, 0);
    }
    else
    {
        ofSetColor(0);
        ofRect(0, 0, moviePlayer->getWidth(), moviePlayer->getHeight());
        ofSetColor(255);
    }
    
    ofEnableAlphaBlending();
    
    if (bShowFlow) {
        ofSetColor(255,255,255,150);
        if (bShowFlowMagnitude) {
            flow.drawMagnitude(0, 0, moviePlayer->getWidth(), moviePlayer->getHeight());
        }
        else if (bShowFlowDirection)
            flow.drawColorFlow(0, 0, moviePlayer->getWidth(), moviePlayer->getHeight());
    }
    
    ofSetColor(255,255,255,255);
    
    eyeMovements->draw(bShowMeanBinocular,
                       bShowEyes,
                       bShowSaccades,
                       bShowHeatmap,
                       bShowDifferenceHeatmap,
                       bShowDetail,
                       bShowNormalized,
                       bShowClustering,
                       bPaused);
    ofDisableAlphaBlending();
    recorderFbo.end();
        
    recorderFbo.draw(0, 0);
    
    
    if(bSaveMovie)
    {
        recorderFbo.readToPixels(readbackPixels);
        unsigned long msPerFrame = 1000.0 / movieFrameRate;
        recorder.addFrame(readbackPixels, msPerFrame);
    }
    
    if (bShowDetail) {
        ofDrawBitmapString("frame: " + ofToString(movieFrameNumber) + "/" + ofToString(movieTotalFrames), ofPoint(20, 20));
        ofDrawBitmapString("stimulus: " + ofToString(currentExperiment+1) + "/" + ofToString(numberOfExperiments), ofPoint(20, 32));
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) { 
    if(key == 'n')
    {
        movieFrameNumber = movieTotalFrames;
        timelinePtr->setCurrentTime(moviePlayer->getDuration());
    }
    else if(key == ' ')
    {
        bPaused = !bPaused;
    }
    else if(key == 'd')
        bShowDetail = !bShowDetail;
    else if(key == '[')
    {
        movieFrameNumber = max(0, movieFrameNumber - 1);
        bNeedsUpdate = true;
    }
    else if(key == ']')
    {
        movieFrameNumber = min(movieTotalFrames - 1, movieFrameNumber + 1);
        bNeedsUpdate = true;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) { 

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) { 

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) { 

}

//--------------------------------------------------------------
void testApp::mouseReleased() { 

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button ) {

}

//--------------------------------------------------------------
void testApp::mouseScrolled(float x, float y) { 

}


//--------------------------------------------------------------
void testApp::saveUserSettings() {
    
}

//--------------------------------------------------------------
void testApp::loadUserSettings() {
    
    //////////////////////////////////////////////////////
    // ask user for xml file
    string xmlURL;
    int OK = ofxFileDialogOSX::openFile(xmlURL, "Open .XML File");
    if (!OK) {
        cout << "[ERROR]: Could not open file dialog!" << endl;
        OF_EXIT_APP(0);
        return;
    }
    
    cout << "[OK]: Choose the xml file: " << xmlURL << endl;
    
    settings.loadFile(xmlURL);
    
    settings.pushTag("globalsettings");
    {
        settings.pushTag("show");
        {
            string logFile = settings.getValue("logfile", "");
            if(logFile != "")
                ofLogToFile(logFile);
            
            bAutoStart = ofToLower(settings.getValue("autostart", "true")) == "true";
            bAutoQuitAfterProcessing = ofToLower(settings.getValue("autoquit", "true")) == "true";
            bShowRealTime = ofToLower(settings.getValue("realtime", "false")) == "true";
            bShowClustering = ofToLower(settings.getValue("clustering", "false")) == "true";
            bShowAlphaScreen = ofToLower(settings.getValue("peekthrough", "false")) == "true";
            bShowContours = ofToLower(settings.getValue("contours", "false")) == "true";
            bShowEyes = ofToLower(settings.getValue("eyes", "true")) == "true";
            bShowSaccades = ofToLower(settings.getValue("saccades", "true")) == "true";
            bShowHeatmap = ofToLower(settings.getValue("heatmap", "true")) == "true";
            heatmapType = std::max<int>(0, std::min<int>(4, settings.getValue("heatmaptype", 4)));
            bShowDifferenceHeatmap = ofToLower(settings.getValue("differenceheatmap", "true")) == "true";
            bShowMeanBinocular = ofToLower(settings.getValue("meanbinocular", "true")) == "true";
            bShowMovie = ofToLower(settings.getValue("movie", "true")) == "true";
            bShowFlow = ofToLower(settings.getValue("visualsaliency", "true")) == "true";
            bShowFlowMagnitude = ofToLower(settings.getValue("visualsaliencymagnitude", "true")) == "true";
            bShowFlowDirection = ofToLower(settings.getValue("visualsaliencydirection", "false")) == "true";
            bShowNormalized = ofToLower(settings.getValue("normalizedheatmap", "true")) == "true";
            bShowRadialBlur = ofToLower(settings.getValue("radialblur", "false")) == "true";
            bShowHistogram = ofToLower(settings.getValue("histogram", "false")) == "true";
            bShowSaccades = ofToLower(settings.getValue("saccades", "false")) == "true";
            bShowSubjectNames = ofToLower(settings.getValue("subjectnames", "false")) == "true";
        }
        settings.popTag();
        settings.pushTag("save");
        {
            saveMovieURL = settings.getValue("location", "");
            bSaveMovie = ofToLower(settings.getValue("movie", "true")) == "true";
            bSaveMovieImages = ofToLower(settings.getValue("images", "false")) == "true";
            bExportMotionDescriptorsToHDF = ofToLower(settings.getValue("motiondescriptors", "true")) == "true";
        }
        settings.popTag();
    }
    settings.popTag();
    
    settings.pushTag("stimuli");
    {
        numberOfExperiments = settings.getNumTags("stimulus");
        currentExperiment = 0;
    }
    settings.popTag();
    
    initializeExperiment();
    initializeVisualSaliency();
    if(bSaveMovie)
        initializeRecording(saveMovieURL);
    
}

//--------------------------------------------------------------
void testApp::initializeExperiment() {
    
    bSetup = false;
    
    settings.pushTag("stimuli");
    {
        settings.pushTag("stimulus", currentExperiment);
        {
            movieName = settings.getValue("name", ofToString(currentExperiment));
            settings.pushTag("presentation");
            {
                screenWidth = settings.getValue("screenwidth", 800);
                screenHeight = settings.getValue("screenheight", 600);
                bCalculateMovieOffset = ofToLower(settings.getValue("centermovie", "true")) == "true";
                movieOffsetX = settings.getValue("movieoffsetx", 0);
                movieOffsetY = settings.getValue("movieoffsety", 0);
            }
            settings.popTag();
            
            settings.pushTag("movie");
            {
                movieFrameRate = settings.getValue("framerate", 30.0f);
                frameIncrement = 1.0;//movieFrameRate / frameIncrement;
                movieURL = settings.getValue("file", "");
                ofLog(OF_LOG_NOTICE, "Reading %s", movieURL.c_str());
            }
            settings.popTag();
            initializeMovie();
            
            settings.pushTag("audio");
            {
                audioURL = settings.getValue("file", "");
                bPlayAudio = ofToLower(settings.getValue("playback", "false")) == "true";
                bSaveAudio = ofToLower(settings.getValue("save", "false")) == "true";
            }
            settings.popTag();
            
            settings.pushTag("eyemovements");
            {
                bLoadBinocular = ofToLower(settings.getValue("binocular", "true")) == "true";
                bLoadMillisecondFormat = ofToLower(settings.getValue("millisecond", "false")) == "true";
                bLoadClassifiedData = ofToLower(settings.getValue("classified", "false")) == "true";
                bLoadArringtonResearchFormat = ofToLower(settings.getValue("arrington", "false")) == "true";
                int numConditions = settings.getNumTags("path");
                cout << "[loadUserSettings()]::[OK] Reading " << numConditions << " conditions." << endl;
                vector<string> paths;
                for (int i = 0; i < numConditions; i++) {
                    paths.push_back(settings.getValue("path", "event_data", i));
                }
                initializeEyeTrackingData(paths);
                eyeMovementFrameRate = settings.getValue("framerate", 1000.0f);
                eyeMovements->setFrameRate(eyeMovementFrameRate);
                eyeMovements->setSigma(settings.getValue("sigma", 50.0));
                
            }
            settings.popTag();
        }
        settings.popTag();
    }
    settings.popTag();
    
    
    if(movieURL != "")
    {
        timelinePtr->setSize(getWidth(), getWidth() * 0.2);
        timelinePtr->addVideo(movieURL);
    }
    if(audioURL != "")
    {
        timelinePtr->setupAudio();
        timelinePtr->addAudio(audioURL, shouldVisualizeAudio(), shouldPlayAudio());
    }
    
    timelinePtr->play();
    ofSetAppWindow(ofxNSWindower::instance()->getWindowPtr("C.A.R.P.E.: Stimulus Display"));
    ofSetWindowShape(movieWidth, movieHeight);
    bSetup = true;
    
}


//--------------------------------------------------------------
void testApp::initializeVisualSaliency()
{
    flow.allocate(moviePlayer->getWidth(), moviePlayer->getHeight(), movieTotalFrames);
}

//--------------------------------------------------------------
void testApp::initializeMovie()
{
    moviePlayer = new ofQTKitPlayer();
    
	//////////////////////////////////////////////////////
    // load the movie file
    if (!moviePlayer->loadMovie(movieURL, OF_QTKIT_DECODE_PIXELS_AND_TEXTURE)) {
        cout << "[ERROR]: Could not load movie: " << movieURL << endl;
        OF_EXIT_APP(0);
        return;
    }
    
	//////////////////////////////////////////////////////
    // set to first frame
    movieFrameNumber = 1;
    moviePlayer->setLoopState(OF_LOOP_NONE);
    moviePlayer->setFrame(movieFrameNumber);
    movieTotalFrames = moviePlayer->getTotalNumFrames();
    
    if (bShowRealTime) {
        moviePlayer->play();
    }
    
    if(movieTotalFrames / (float) moviePlayer->getDuration() != movieFrameRate)
    {
        cout << "[WARNING] User XML Settings declared movie frame rate as " << movieFrameRate << " though detected in movie file as " << movieTotalFrames / (float) moviePlayer->getDuration() << ".  Defaulting to user setting." << endl;
    }
    movieWidth = moviePlayer->getWidth();
    movieHeight = moviePlayer->getHeight();
    movieMSPerFrame = 1.0 / movieFrameRate * 1000.0;
    cout << "[OK]: Loaded movie with " << movieTotalFrames << " frames at " << movieFrameRate << " fps and " << movieWidth << "x" << movieHeight << " resolution." << endl;
    
	//////////////////////////////////////////////////////
    // calculate offset
    if (bCalculateMovieOffset) {
        movieOffsetX = (screenWidth - movieWidth) / 2.0;
        movieOffsetY = (screenHeight - movieHeight) / 2.0;
    }
}

//--------------------------------------------------------------
void testApp::initializeRecording(string location)
{
	//////////////////////////////////////////////////////
    // set codec
    //recorder.listCodecs();
    //recorder.setCodecPhotoJpeg();
    //recorder.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_HIGH);
	
	//////////////////////////////////////////////////////
	// Output movie to
	int fileno = 1;
	ifstream in;
	do {
		in.close();
		stringstream str;
		// format the filename here
		if(fileno == 1)
            str << location << "/" << movieName << "_output" << ".mov";
        //str << "output/" << movieName << "/" << movieName << "_output" << ".mov";
		else
            str << location << "/" << movieName << "_output" << "(" << (fileno-1) << ").mov";
        //str << "output/" << movieName << "/" << movieName << "_output" << "(" << (fileno-1) << ").mov";
		saveFilename = str.str();
		++fileno;
		// attempt to open for read
		in.open( saveFilename.c_str() );
	} while( in.is_open() );
	in.close();
	// found a file that does not exists
    
	//////////////////////////////////////////////////////
	// now create the file so that we can start adding frames to it:
	ofstream tmpptr( saveFilename.c_str() );
	tmpptr.close();
    
	//////////////////////////////////////////////////////
    // setup recorder

    recorder.setup(saveFilename);
    //recorder.loadAudioTrack(ofToDataPath(out_movie_audio, true).c_str());
    //cout << "Adding audio track: " << ofToDataPath(out_movie_audio) << endl;
    recorderFbo.allocate(movieWidth, movieHeight, GL_RGB);

    
	//////////////////////////////////////////////////////
    // setup asynchronous readback
    asyncReadingIndex = 0;
    asyncCopyingIndex = 1;
    pboIds = new GLuint[2];
    pboIds[0] = 0; pboIds[1] = 0;
    const int DATA_SIZE = movieWidth * movieHeight * 4;
    const int PBO_COUNT = 2;
    glGenBuffersARB(PBO_COUNT, pboIds);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[0]);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_READ_ARB);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[1]);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_READ_ARB);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
    
    const int buffersize = movieWidth * movieHeight * 3;
    readbackPixelsBuffer = new unsigned char[buffersize];
    readbackPixels.allocate(movieWidth, movieHeight, 3);
    readbackPixels.setImageType(OF_IMAGE_COLOR);
    
    bDoneRecording = false;
}

//--------------------------------------------------------------
void testApp::initializeEyeTrackingData(vector<string> paths)
{
    eyeMovements = new pkmEyeMovementCollection();
    eyeMovements->allocateHeatmap(movieWidth, movieHeight, movieOffsetX, movieOffsetY);
    for(int i = 0; i < paths.size(); i++)
    {
        if(bLoadArringtonResearchFormat)
            eyeMovements->loadArringtonFiles(paths[i]);
        else
            //eyeMovements->loadFiles(paths[i], movieName, bLoadBinocular, i);
            if(heatmapType == 4)
                if(paths.size() > 1)
                    eyeMovements->loadFiles(paths[i], bLoadBinocular, bLoadMillisecondFormat, bLoadClassifiedData, i, i+1);
                else
                    eyeMovements->loadFiles(paths[i], bLoadBinocular, bLoadMillisecondFormat, bLoadClassifiedData, i, 0);
            else
                eyeMovements->loadFiles(paths[i], bLoadBinocular, bLoadMillisecondFormat, bLoadClassifiedData, i, heatmapType);
    }
}

void testApp::initializeTimeline()
{
    
}

//--------------------------------------------------------------
void testApp::exportMotionDescriptorsToHDF5()
{
//    flow.exportHOMGToHDF5();                                  // HDF5 broken in universal build...
//    flow.exportHOMGToPPM(saveMovieURL + "/", movieName);      // not enough precision
    flow.exportHOMGToYML(saveMovieURL + "/", movieName);        // requires importing opencv into python...
}


//--------------------------------------------------------------
void testApp::exportFrame()
{
    
}