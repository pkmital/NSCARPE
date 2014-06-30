
#include "testApp.h"
#include "ofxFileDialogOSX.h"

//--------------------------------------------------------------
void testApp::setup() {
    bSetup = false;
    
    ofSetLogLevel(OF_LOG_ERROR);
    ofEnableArbTex();
    ofEnableDataPath();
    
    loadUserSettings();

	ofSetWindowShape(movieWidth, movieHeight);
    bSetup = true;
}

//--------------------------------------------------------------
void testApp::close() {
    saveUserSettings();
    recorder.flushToFile();
    recorder.releaseRecording();
}

//--------------------------------------------------------------
void testApp::update() {
    if(!bSetup)
    {
        return;
    }
    
    if (movieFrameNumber > movieTotalFrames)
    {
        ofLog(OF_LOG_NOTICE, "Reached end of movie at %f seconds. Exiting.", movieFrameNumber / (float)movieFrameRate);
        close();
        OF_EXIT_APP(0);
    }
    
    moviePlayer.setFrame(movieFrameNumber);
    eyeMovements.setTime(movieFrameNumber / (float)movieFrameRate);

    movieFrameNumber+=frameIncrement;
    
    ofLog(OF_LOG_NOTICE, "Processing Frame %f/%f", movieFrameNumber, movieTotalFrames);
}

//--------------------------------------------------------------
void testApp::draw() 
{
    if(!bSetup)
    {
        return;
    }
    
    recorderFbo.begin();
    moviePlayer.draw(0, 0);
    ofEnableAlphaBlending();
    eyeMovements.draw(true);
    ofDisableAlphaBlending();
    recorderFbo.end();
    recorderFbo.draw(0, 0);
    
    recorderFbo.readToPixels(readbackPixels);
    recorder.addFrame(readbackPixels, movieFrameRate);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) { 

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
void testApp::mouseReleased(int x, int y, int button ){ 

}

//--------------------------------------------------------------
void testApp::mouseScrolled(float x, float y) { 

}


//--------------------------------------------------------------
void testApp::saveUserSettings()
{
    
}



//--------------------------------------------------------------
void testApp::loadUserSettings()
{
    
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
    
    ofxXmlSettings settings;
    settings.loadFile(xmlURL);
    
    
    settings.pushTag("show");
    {
        bShowRealTime = settings.getValue("realtime", false);
        bShowClustering = settings.getValue("clustering", false);
        bShowAlphaScreen = settings.getValue("peekthrough", false);
        bShowContours = settings.getValue("contours", false);
        bShowEyes = settings.getValue("eyes", true);
        bShowHeatmap = settings.getValue("heatmap", true);
        bShowMeanBinocular = settings.getValue("meanbinocular", true);
        bShowMovie = settings.getValue("movie", true);
        bShowNormalized = settings.getValue("normalizedheatmap", true);
        bShowRadialBlur = settings.getValue("radialblur", false);
        bShowHistogram = settings.getValue("histogram", false);
        bShowSaccades = settings.getValue("saccades", false);
        bShowSubjectNames = settings.getValue("subjectnames", false);
        frameIncrement = settings.getValue("framerate", 25.0f);
    }
    settings.popTag();
    
    settings.pushTag("presentation");
    {
        screenWidth = settings.getValue("screenwidth", 800);
        screenHeight = settings.getValue("screenheight", 600);
        bCalculateMovieOffset = settings.getValue("centermovie", true);
        movieOffsetX = settings.getValue("movieoffsetx", 0);
        movieOffsetY = settings.getValue("movieoffsety", 0);
    }
    settings.popTag();
    
    settings.pushTag("save");
    {
        //bSaveMovie = settings.getValue("movie", true);
        bSaveMovieImages = settings.getValue("images", false);
    }
    settings.popTag();
    
    string movieURL;
    settings.pushTag("movie");
    {
        movieFrameRate = settings.getValue("framerate", 25.0f);
        frameIncrement = movieFrameRate / frameIncrement;
        movieURL = settings.getValue("file", "");
    }
    settings.popTag();
    initializeMovie(movieURL);
    
    settings.pushTag("eyemovements");
    {
        bLoadBinocular = settings.getValue("binocular", true);
        bLoadClassifiedData = settings.getValue("classified", false);
        bLoadArringtonResearchFormat = settings.getValue("arrington", false);
        eyeMovementFrameRate = settings.getValue("framerate", 1000.0f);
        eyeMovements.setFrameRate(eyeMovementFrameRate);
        int numConditions = settings.getNumTags("path");
        cout << "[loadUserSettings()]::[OK] Reading " << numConditions << " conditions." << endl;
        vector<string> paths;
        for (int i = 0; i < numConditions; i++) {
            paths.push_back(settings.getValue("path", "event_data", i));
        }
        eyeMovements.setSigma(settings.getValue("sigma", 50));
        initializeEyeTrackingData(paths);
        
    }
    settings.popTag();
    
    initializeRecording();
    
    
}

//--------------------------------------------------------------
void testApp::initializeMovie(string movieURL)
{
	//////////////////////////////////////////////////////
    // load the movie file
    if (!moviePlayer.loadMovie(movieURL, OF_QTKIT_DECODE_TEXTURE_ONLY)) {
        cout << "[ERROR]: Could not load movie: " << movieURL << endl;
        OF_EXIT_APP(0);
        return;
    }
    
	//////////////////////////////////////////////////////
    // set to first frame
    movieFrameNumber = 1;
    moviePlayer.setLoopState(OF_LOOP_NONE);
    moviePlayer.setFrame(movieFrameNumber);
    movieTotalFrames = moviePlayer.getTotalNumFrames();
    if(movieTotalFrames / (float) moviePlayer.getDuration() != movieFrameRate)
    {
        cout << "[WARNING] User XML Settings declared movie frame rate as " << movieFrameRate << " though detected in movie file as " << movieTotalFrames / (float) moviePlayer.getDuration() << ".  Defaulting to user setting." << endl;
    }
    movieWidth = moviePlayer.getWidth();
    movieHeight = moviePlayer.getHeight();
    movieMSPerFrame = 1.0 / movieFrameRate * 1000;
    cout << "[OK]: Loaded movie with " << movieTotalFrames << " frames at " << movieFrameRate << " fps and " << movieWidth << "x" << movieHeight << " resolution." << endl;
    
	//////////////////////////////////////////////////////
    // calculate offset
    if (bCalculateMovieOffset) {
        movieOffsetX = (screenWidth - movieWidth) / 2.0;
        movieOffsetY = (screenHeight - movieHeight) / 2.0;
    }
}

//--------------------------------------------------------------
void testApp::initializeRecording()
{
	//////////////////////////////////////////////////////
    // set codec
    //recorder.listCodecs();
    //recorder.setCodecPhotoJpeg();
    //recorder.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_HIGH);
	
	//////////////////////////////////////////////////////
	// Output movie to
	string filename;
	int fileno = 1;
	ifstream in;
	do {
		in.close();
		stringstream str;
		// format the filename here
		if(fileno == 1)
            str << "output/" << movieName << "_output" << ".mov";
        //str << "output/" << movieName << "/" << movieName << "_output" << ".mov";
		else
            str << "output/" << movieName << "_output" << "(" << (fileno-1) << ").mov";
        //str << "output/" << movieName << "/" << movieName << "_output" << "(" << (fileno-1) << ").mov";
		filename = str.str();
		++fileno;
		// attempt to open for read
		in.open( ofToDataPath(filename).c_str() );
	} while( in.is_open() );
	in.close();
	// found a file that does not exists
    
	//////////////////////////////////////////////////////
	// now create the file so that we can start adding frames to it:
	ofstream tmpptr( ofToDataPath(filename).c_str() );
	tmpptr.close();
    
	//////////////////////////////////////////////////////
    // setup recorder
	string out_movie_audio = "audio/" + movieName + ".wav";
    recorder.setup(ofToDataPath(filename, true));
    //recorder.addAudioTrack(ofToDataPath(out_movie_audio, true).c_str());
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
    eyeMovements.allocateHeatmap(movieWidth, movieHeight, movieOffsetX, movieOffsetY);
    for(int i = 0; i < paths.size(); i++)
    {
        if(bLoadArringtonResearchFormat)
            eyeMovements.loadArringtonFiles(paths[i]);
        else
            //eyeMovements.loadFiles(paths[i], movieName, bLoadBinocular, i);
            eyeMovements.loadFiles(paths[i], bLoadBinocular, i);
    }
}


