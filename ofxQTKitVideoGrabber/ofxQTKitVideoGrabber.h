/**
 * QTKit / CoreVideo based Video Grabber for openFrameworks 
 *
 * Copyright 2010 (c) James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 *
 * Video & Audio sync'd recording + named device id's 
 * added by gameover [matt gingold] (c) 2011 http://gingold.com.au
 * with the support of hydra poesis http://hydrapoesis.net
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxQTKitVideoGrabber works exactly the same as the standard ofMovieGrabber
 * but uses the QTKit Objective-C Libraries to drive the video display.
 * These libraries are naturally GPU enabled, multi-threaded, as well
 * as supporting more Quicktime capture codecs such as HDV.
 *
 * You will need to add the QTKit.framework and CoreVideo.framework
 * to the openFrameworks Xcode project
 *
 * Requires Mac OS 10.5 or greater
 * 
 */


//  Modified by Seth Hunter (hunters@mit.edu) on 8/14/12. 
//  2012 MIT Media Lab - use it as you please. 
//

#ifndef _OFX_QTKITVIDEOGRABBER
#define _OFX_QTKITVIDEOGRABBER

#include "ofMain.h"

//using this #ifdef lets this .h file be included in cpp files without throwing errors
//but when included in .m files it works correctly.  another useful trick for mixing oF/ObjC
#ifdef __OBJC__
@class QTKitVideoGrabber;
#endif

class ofxQTKitVideoGrabber : public ofBaseVideo
{
  public:
	ofxQTKitVideoGrabber();
	~ofxQTKitVideoGrabber();

#if OF_VERSION == 7
	ofPixelsRef		getPixelsRef(){ofPixels p; return p;}; //TODO: make this work (satisfies abstract class requirement)
#endif
	
	void			initGrabber(int w, int h, bool buseAudio = false);
	void			grabFrame();
	bool			isFrameNew();
	void			update();
	void 			setUseTexture(bool bUse);
	
	vector<string>*	listDevices();
	vector<string>*	listAudioDevices();
	vector<string>*	listVideoDevices();
	void			close();
	unsigned char 	* getPixels();
	ofTexture &		getTextureReference();
	void 			setVerbose(bool bTalkToMe);
	void			setDeviceID(int videoDeviceID);
	void			setDeviceID(string videoDeviceIDString);
	int				getDeviceID();
	void			setVideoDeviceID(int videoDeviceID);
	void			setVideoDeviceID(string videoDeviceIDString);
	int				getVideoDeviceID();
	void			setAudioDeviceID(int audioDeviceID);
	void			setAudioDeviceID(string audioDeviceIDString);
	int				getAudioDeviceID();
	void			setDesiredFrameRate(int framerate){ ofLog(OF_LOG_WARNING, "ofxQTKitVideoGrabber -- Cannot specify framerate.");  };

	void			videoSettings();
	void			audioSettings();
	
	// [added by gameover] - For saving to a file... 
	void			initRecording();
    void            initAudioRecording();
	void			initGrabber(bool bUseAudio = true);	// used to init with no preview/textures etc
    void			initAudioGrabber();	// for just an audio track
	vector<string>*	listVideoCodecs();
	vector<string>*	listAudioCodecs();
	void			setVideoCodec(string videoCodecIDString);
	void			setAudioCodec(string audioCodecIDString);
	void			startRecording(string filePath);
	void			stopRecording();
	bool			isRecording();
	bool			isReady();
	
	void 			draw(float x, float y, float w, float h);
	void 			draw(float x, float y);
	
	float 			getHeight();
	float 			getWidth();
	
  protected:

	vector<string>*	videoDeviceVec;
	vector<string>*	audioDeviceVec;
	
	vector<string>*	videoCodecsVec;
	vector<string>*	audioCodecsVec;
	
	bool	confirmInit();
	int		videoDeviceID;
	int		audioDeviceID;
	int		videoDeviceIDString;
	int		audioDeviceIDString;
	string	videoCodecIDString;
	string	audioCodecIDString;
	bool	useAudio;
	bool	isInited;
	bool	bUseTexture;
	
	#ifdef __OBJC__
	QTKitVideoGrabber* grabber; //only obj-c needs to know the type of this protected var
	#else
	void* grabber;
	#endif
};

#endif