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


//
//  pkmEyeMovementCollection.h
//  CARPE
//
//  Created by Parag Mital on 10/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
//#include "pkmHeatmap.h"
#include "pkmMixtureOfGaussiansHeatmap.h"
#include "pkmDPGMM.h"

//--------------------------------------------------------------
class pkmEyeMovementSubject {
public:
    //--------------------------------------------------------------
    int                 numFramesRecorded;
    vector<ofVec2f>     monocularEye;
    vector<ofVec2f>     binocularLEye;
    vector<ofVec2f>     binocularREye;
    //    vector<float>       dilationL;
    //    vector<float>       dilationR;
    vector<ofVec2f>     binocularMeanEye;
    //    vector<float>       binocularMeanDilation;
    //    vector<int>         eventL;
    //    vector<int>         eventR;
    vector<int>         classification;
    vector<bool>        bFixation;
    //    vector<int>         frameNumber;
    map<int,vector<int> >        msLookUp;
    vector<ofVec2f>     previousEye;
    string              subjectName;
    float               radius;
};
//--------------------------------------------------------------

//--------------------------------------------------------------
class pkmEyeMovementCollection {
    
public:
    //--------------------------------------------------------------
    pkmEyeMovementCollection()
    {
        totalClasses            = 0;
        offsetX                 = 0;
        offsetY                 = 0;
        currentFrame            = 0;
        numSubjects             = 0;
        numConditions           = 0;
        frameRate               = 0;
        sigmaInPixels           = 0;
        maxValue1 = maxValue2   = 1.0;
        
        
        bMultipleHeatmaps       = false;
        bNeedsUpdate            = false;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    ~pkmEyeMovementCollection()
    {
        for(vector<vector<pkmEyeMovementSubject> >::iterator it = subjects.begin();
            it != subjects.end();
            it++)
        {
            it->erase(it->begin(), it->end());
        }
        
        multipleHeatmaps.erase(multipleHeatmaps.begin(), multipleHeatmaps.end());
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void allocateHeatmap(int movieWidth, int movieHeight, int offsetX, int offsetY)
    {
        this->movieWidth = movieWidth;
        this->movieHeight = movieHeight;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        
        reduction.allocate(movieWidth, movieHeight);
        
        ofLog(OF_LOG_NOTICE, "Loading FBO");
        
        ofFbo::Settings settings;
        settings.minFilter = GL_LINEAR;
        settings.maxFilter = GL_LINEAR;
        settings.width = movieWidth;
        settings.height = movieHeight;
        settings.internalformat = GL_RGBA32F_ARB;
        settings.numSamples = 4;
        settings.useDepth = false;
        settings.useStencil = false;
        
        heatmapFBO.allocate(settings);
        heatmap1.allocate(settings);
        heatmap2.allocate(settings);
        
        heatmapFBO.begin();
        glClear( GL_COLOR_BUFFER_BIT );
        heatmapFBO.end();
        
        heatmap1.begin();
        glClear( GL_COLOR_BUFFER_BIT );
        heatmap1.end();
        
        heatmap2.begin();
        glClear( GL_COLOR_BUFFER_BIT );
        heatmap2.end();
        
        ofLog(OF_LOG_NOTICE, "Loading Mix Shader");
        mixShader.load(ofToDataPath("mix 2", true));
        ofLog(OF_LOG_NOTICE, "Loading Difference of Heatmap Shader");
        diffShader.load(ofToDataPath("diffheatmap", true));
        ofLog(OF_LOG_NOTICE, "Loading Norm Shader");
        normShader.load(ofToDataPath("norm", true));
        ofLog(OF_LOG_NOTICE, "Loading Add Shader");
        addShader.load(ofToDataPath("add", true));
        ofLog(OF_LOG_NOTICE, "Loading Subtract Shader");
        subtractShader.load(ofToDataPath("subtract", true));
        ofLog(OF_LOG_NOTICE, "Loading Jet Shader");
        jetShader.load(ofToDataPath("jetmap", true));
        
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void loadArringtonFiles(string path, int colormapType = 0)
    {
        // create new vector of subjects for condition
        vector<pkmEyeMovementSubject> a;
        subjects.push_back(a);
        int conditionNumber = subjects.size() - 1;
        int subject_i = numSubjects;
        numSubjects++;
        subjects[conditionNumber].resize(numSubjects);
        
        // create new heatmap for condition
        ofPtr<pkmMixtureOfGaussiansHeatmap> h(new pkmMixtureOfGaussiansHeatmap());
        h->allocate(movieWidth, movieHeight);
        h->setColorMap((pkmMixtureOfGaussiansHeatmap::colormode)colormapType);
        multipleHeatmaps.push_back(h);
        totalClasses++;
        
        // load file
        ofLog(OF_LOG_NOTICE, "Loading Eye-Movement Files");
        ifstream eyeFile;
        eyeFile.open(path.c_str(), ifstream::in);
        if(!eyeFile.is_open())
            ofLog(OF_LOG_ERROR, "Could not open %s", path.c_str());
        
        int frameCounter = 0;
        int msCounter = 0;
        
        string line;
        int format;
        char buf[256];
        float ms;
        int flag;
        int frame;
        float startMS = 0;
        while (!eyeFile.eof())
        {
            try
            {
                getline(eyeFile, line);
                sscanf(line.c_str(), "%d %s", &format, buf);
                
                if(format == 777)
                {
                    //777	  0.0577	MovieFrame	1	0
                    sscanf(line.c_str(), "%d %f %s %d %d", &format, &ms, buf, &flag, &frame);
                    if(frame == 0)
                        startMS = ms;
                    //cout << "Frame " << frame << endl;
                    
                    getline(eyeFile, line);
                    sscanf(line.c_str(), "%d %s", &format, buf);
                    if(format == 10)
                    {
                        //TotalTime	DeltaTime	X_Gaze	Y_Gaze	Region	PupilWidth	PupilHeight	Quality	Fixation	TotalTime	DeltaTime	X_Gaze	Y_Gaze	Region	PupilWidth	PupilHeight	Quality	Fixation	 X 	 Y 	 Z 	 yaw 	 pitch 	 roll 	Count	Marker
                        //10	  0.0617	  10.7620	0.4296	0.5973	-1	0.1204	0.1124	1	10.9790	  0.0606	  10.1670	0.5736	0.5683	1	0.1014	0.0935	1	10.9624	  0.00	  0.00	  0.00	  0.00	  0.00	  0.00	3
                        int code;
                        float totalTimeL, deltaTimeL, xGazeL, yGazeL;
                        int regionL;
                        float pupilWidthL, pupilHeightL;
                        int qualityL;
                        float fixationL;
                        float totalTimeR, deltaTimeR, xGazeR, yGazeR;
                        int regionR;
                        float pupilWidthR, pupilHeightR;
                        int qualityR;
                        float fixationR;
                        float headX, headY, headZ, headYaw, headPitch, headRoll;
                        int count;
                        sscanf(line.c_str(), "%d %f %f %f %f %d %f %f %d %f %f %f %f %f %d %f %f %d %f %f %f %f %f %f %f %d",
                               &code,
                               &totalTimeL, &deltaTimeL, &xGazeL, &yGazeL,
                               &regionL, &pupilWidthL, &pupilHeightL, &qualityL, &fixationL,
                               &totalTimeR, &deltaTimeR, &xGazeR, &yGazeR,
                               &regionR, &pupilWidthR, &pupilHeightR, &qualityR, &fixationR,
                               &headX, &headY, &headZ, &headYaw, &headPitch, &headRoll, &count);
                        
                        //cout << xGazeL << "," << yGazeL << "; " << xGazeR << "," << yGazeR << endl;
                        
                        float x, x2, y, y2;
                        x = xGazeL * movieWidth;
                        y = yGazeL * movieHeight;
                        x2 = xGazeR * movieWidth;
                        y2 = yGazeR * movieHeight;
                        int msCounter = (totalTimeL - startMS) * 1000.0;
                        cout << msCounter << endl;
                        subjects[conditionNumber][subject_i].binocularLEye.push_back(ofVec2f(x, y));
                        subjects[conditionNumber][subject_i].binocularREye.push_back(ofVec2f(x2, y2));
                        subjects[conditionNumber][subject_i].binocularMeanEye.push_back(ofVec2f((x + x2) / 2.0,
                                                                                                (y + y2) / 2.0));
                        subjects[conditionNumber][subject_i].bFixation.push_back(true);
                        subjects[conditionNumber][subject_i].msLookUp[msCounter].push_back(subjects[conditionNumber][subject_i].binocularLEye.size());
                    }
                }
                
                frameCounter++;
            }
            catch(...)
            {
                
            }
            subjects[conditionNumber][subject_i].radius = 5;
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void loadFiles(string path,                         // location of files
                   //string token = "",                   // token of the files in the directory to load
                   bool bLoadBinocular = true,          // format of eye-data has twice as many columns for the second eye
                   bool bLoadMillisecondData = false,   // 1st column has millisecond frame, 2nd column has video frame.
                   bool bLoadClassifiedData = false,
                   int conditionNumber = 0,
                   int colormapType = 0)     // (for multiple heatmaps)
    {
        bMultipleHeatmaps = true;
        
        ofDirectory eyeDir;
        eyeDir.allowExt("txt");
        //if (token.size() == 0) {
            numSubjects = eyeDir.listDir(ofToDataPath(path, true));
        //}
        //else {
        //    // added listDir(string, string) to ofDirectory.h
        //    numSubjects = eyeDir.listDir(ofToDataPath(path, true), token);
        //}
        if (numSubjects <= 0) {
            cout << "[ERROR]: No data found in " << ofToDataPath(path, true) << endl;
            OF_EXIT_APP(0);
            return;
        }
        
        // This is really experimental.. need a test case for multiple conditions... 
        // create new vector of subjects for condition
        vector<pkmEyeMovementSubject> a;
        a.resize(numSubjects);
        subjects.push_back(a);
        
        // create new heatmap for condition
        ofPtr<pkmMixtureOfGaussiansHeatmap> h(new pkmMixtureOfGaussiansHeatmap());
        h->allocate(movieWidth, movieHeight);
        h->setColorMap((pkmMixtureOfGaussiansHeatmap::colormode)(colormapType));
        multipleHeatmaps.push_back(h);
        totalClasses++;
        
        ofLog(OF_LOG_NOTICE, "Loading Eye-Movement Files");
        for (int subject_i = 0; subject_i < numSubjects; subject_i++)
        {
            string filename = eyeDir.getPath(subject_i);
            cout << subject_i << ": " << filename << endl;
            ifstream eyeFile;
            eyeFile.open(filename.c_str(), ifstream::in);
            
            if(!eyeFile.is_open())
                ofLog(OF_LOG_ERROR, "Could not open %s", path.c_str());
            
            int idx1 = filename.rfind(string("/"));
            int idx2 = filename.find(string("."), idx1);
            if(idx2 > idx1)
                subjects[conditionNumber][subject_i].subjectName = filename.substr(idx1 + 1, idx2 - idx1 - 1);
            
            int frameCounter = 0;
            int msCounter = 0;
            while (!eyeFile.eof())
            {
                try
                {
                    // parse the values in the eye-tracking files,
                    // and store in the current eyeList[] object
                    float x, y, x2, y2, dil, dil2;
                    int left_event, right_event;
                    if(bLoadBinocular)
                    {
                        // [(optional)ms] [frame] [left_x] [left_y] [left_dil] [left_event] [right_x] [right_y] [right_dil] [right_event] [(optional)condition_number]
                        if(bLoadMillisecondData)
                            eyeFile >> msCounter >> frameCounter >> x >> y >> dil >> left_event >> x2 >> y2 >> dil2 >> right_event;
                        else
                            eyeFile >> frameCounter >> x >> y >> dil >> left_event >> x2 >> y2 >> dil2 >> right_event;
                        
                        if(bLoadClassifiedData)
                        {
                            eyeFile >> conditionNumber;
                            subjects[conditionNumber][subject_i].classification.push_back(conditionNumber);
                        }
                        
                        subjects[conditionNumber][subject_i].binocularLEye.push_back(ofVec2f(x,y));
                        subjects[conditionNumber][subject_i].binocularREye.push_back(ofVec2f(x2,y2));
                        //                    subjects[conditionNumber][subject_i].eventL.push_back(left_event);
                        //                    subjects[conditionNumber][subject_i].eventR.push_back(right_event);
                        //                    subjects[conditionNumber][i].dilationL.push_back(dil);
                        //                    subjects[conditionNumber][i].dilationR.push_back(dil2);
                        subjects[conditionNumber][subject_i].binocularMeanEye.push_back(ofVec2f((x + x2) / 2.0,
                                                                                                (y + y2) / 2.0));
                        //                    subjects[conditionNumber][i].binocularMeanDilation.push_back((dil + dil2) / 2.0);
                        subjects[conditionNumber][subject_i].bFixation.push_back(right_event == 1 && left_event == 1);
                        if(bLoadMillisecondData)
                            subjects[conditionNumber][subject_i].msLookUp[frameCounter].push_back(msCounter);
                        else
                            subjects[conditionNumber][subject_i].msLookUp[frameCounter].push_back(subjects[conditionNumber][subject_i].binocularLEye.size());
                        
                    }
                    else
                    {
                        ofLog(OF_LOG_ERROR, "[ERROR]: Monocular loading not yet supported!");
                        OF_EXIT_APP(0);
                        return;
                    }
                    frameCounter++;
                }
                catch(...)
                {
                    ofLog(OF_LOG_ERROR, "[ERROR] Could not parse text file.  Error found at line %d",msCounter);
                    OF_EXIT_APP(0);
                    return;
                }
			}
            subjects[conditionNumber][subject_i].numFramesRecorded = frameCounter;
            subjects[conditionNumber][subject_i].radius = 5;
			
            // close the file after reading it
			eyeFile.close();
        }
        ofLog(OF_LOG_NOTICE, "Loaded %d files for condition %d", numSubjects, conditionNumber);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setSigma(float varianceInPixels)
    {
        sigmaInPixels = varianceInPixels;
    }
    //--------------------------------------------------------------
    
    
    //--------------------------------------------------------------
    void setFrameRate(float framesPerSecond)
    {
        frameRate = framesPerSecond;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setTime(float seconds)
    {
        setFrame(seconds * frameRate);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setFrame(unsigned long frame)
    {
        if (frame != currentFrame) {
            bNeedsUpdate = true;
        }
        
        currentFrame = frame;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    // can draw all ms data for a single frame? as path?
    void draw(int numFrames, bool bDrawMeanEye, bool bDrawEyes, bool bDrawSaccades, bool bDrawHeatmaps, bool bDrawDifferenceHeatmap, bool bDrawDetail, bool bShowNormalized, bool bDrawClustering, bool bPaused)
    {
        if (bNeedsUpdate)
        {
            assert(subjects.size() == multipleHeatmaps.size());
            
            ofNoFill();
            for(int condition_i = 0; condition_i < subjects.size(); condition_i++)
            {
                ofSetColor(condition_i*255, 255 * (1 - condition_i), 0.0);
                
                // reset heatmap
                multipleHeatmaps[condition_i]->resetMixture();
                
                // add points to heatmap condition for all subjects
                ofVec2f sigma;
                sigma.x = sigmaInPixels;
                sigma.y = sigmaInPixels;
                float weight = 1.0f; // (float)subjects[condition_i].size();
                vector<pkmEyeMovementSubject>::iterator subject = subjects[condition_i].begin();
                while(subject != subjects[condition_i].end())
                {
                    if (subject->msLookUp.find(currentFrame) != subject->msLookUp.end())
                    {
                        for(vector<int>::iterator frame_it = subject->msLookUp[currentFrame].begin();
                            frame_it != subject->msLookUp[currentFrame].end() &&
                            subject->bFixation[*frame_it];
                            frame_it++)
                        {
                            int subjectFrameIdx = *frame_it;
                            
                            if (bDrawMeanEye)
                            {
                                multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                                                                subject->binocularMeanEye[subjectFrameIdx].y - offsetY),
                                                                        sigma,
                                                                        weight);
                                
                                if(!bPaused)
                                {
                                    if (subjectFrameIdx > 0 &&
                                        ofDist(subject->binocularMeanEye[subjectFrameIdx].x,
                                               subject->binocularMeanEye[subjectFrameIdx].y,
                                               subject->binocularMeanEye[subjectFrameIdx-1].x,
                                               subject->binocularMeanEye[subjectFrameIdx-1].y) < (sigmaInPixels * 0.25))
                                        subject->radius += 0.5;
                                    else
                                        subject->radius = 5;
                                }
                                
                                if (bDrawSaccades && subjectFrameIdx > 2) {
                                    ofLine(subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                           subject->binocularMeanEye[subjectFrameIdx].y - offsetY,
                                           subject->binocularMeanEye[subjectFrameIdx-1].x - offsetX,
                                           subject->binocularMeanEye[subjectFrameIdx-1].y - offsetY);
                                    ofLine(subject->binocularMeanEye[subjectFrameIdx-2].x - offsetX,
                                           subject->binocularMeanEye[subjectFrameIdx-2].y - offsetY,
                                           subject->binocularMeanEye[subjectFrameIdx-1].x - offsetX,
                                           subject->binocularMeanEye[subjectFrameIdx-1].y - offsetY);
                                }
                                
                                if(bDrawEyes)
                                {
                                    ofFill();
                                    ofCircle(subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                             subject->binocularMeanEye[subjectFrameIdx].y - offsetY,
                                             subject->radius);
                                    ofNoFill();
                                    ofCircle(subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                             subject->binocularMeanEye[subjectFrameIdx].y - offsetY,
                                             subject->radius);
                                    
                                    
                                }
                                
                                if(bDrawDetail)
                                {
                                    
                                    ofDrawBitmapString(ofToString((int)subject->binocularMeanEye[subjectFrameIdx].x - offsetX) + "," + ofToString((int)subject->binocularMeanEye[subjectFrameIdx].y - offsetY),
                                                       subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                                       subject->binocularMeanEye[subjectFrameIdx].y - offsetY);
                                }
                                
                            }
                            else
                            {
                                multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularLEye[subjectFrameIdx].x - offsetX,
                                                                                subject->binocularLEye[subjectFrameIdx].y - offsetY),
                                                                        sigma,
                                                                        weight);
                                
                                ofCircle(subject->binocularLEye[subjectFrameIdx].x - offsetX,
                                         subject->binocularLEye[subjectFrameIdx].y - offsetY,
                                         sigmaInPixels);
                                
                                multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularREye[subjectFrameIdx].x - offsetX,
                                                                                subject->binocularREye[subjectFrameIdx].y - offsetY),
                                                                        sigma,
                                                                        weight);
                                
                                ofCircle(subject->binocularREye[subjectFrameIdx].x - offsetX,
                                         subject->binocularREye[subjectFrameIdx].y - offsetY,
                                         sigmaInPixels);
                            }
                        }
                    }
                    
                    subject++;
                }
                
            }
            ofSetColor(255);
            ofFill();
            

//            multipleHeatmaps[0]->buildMixture();
//            multipleHeatmaps[0]->update();
//            multipleHeatmaps[1]->buildMixture();
//            multipleHeatmaps[1]->update();
//            
//            heatmapFBO.begin();
//            glClear(GL_COLOR_BUFFER_BIT);
//            multipleHeatmaps[0]->draw();
//            multipleHeatmaps[1]->draw();
//            heatmapFBO.end();
//            heatmapFBO.draw(0, 0);


            if(bDrawHeatmaps)
            {
                multipleHeatmaps[0]->buildMixture();
                if(bShowNormalized)
                    multipleHeatmaps[0]->normalizeHeatmap();
                multipleHeatmaps[0]->update(bDrawClustering);
                if(totalClasses > 1)
                {
                    multipleHeatmaps[1]->buildMixture();
                    if(bShowNormalized)
                        multipleHeatmaps[1]->normalizeHeatmap();
                    multipleHeatmaps[1]->update(bDrawClustering);
                    
                    heatmapFBO.begin();
                    glClear(GL_COLOR_BUFFER_BIT);
                    
                    if(bDrawDifferenceHeatmap)
                    {
                        diffShader.begin();
                        diffShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
                        diffShader.setUniform1f("maxValue1", multipleHeatmaps[0]->getMaxValue());

                        diffShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
                        diffShader.setUniform1f("maxValue2", multipleHeatmaps[1]->getMaxValue());

                        multipleHeatmaps[0]->draw();
                        diffShader.end();
                    }
                    else
                    {
                        mixShader.begin();
                        mixShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
                        mixShader.setUniform1f("maxValue1", multipleHeatmaps[0]->getMaxValue());

                        mixShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
                        mixShader.setUniform1f("maxValue2", multipleHeatmaps[1]->getMaxValue());

                        multipleHeatmaps[0]->draw();
                        mixShader.end();
                    }
                    
                    heatmapFBO.end();
                }
                else
                {
                    heatmapFBO.begin();
                    glClear(GL_COLOR_BUFFER_BIT);
                    multipleHeatmaps[0]->draw();
                    heatmapFBO.end();
                }
            }
            
            bNeedsUpdate = false;
        }
        
        if(bDrawHeatmaps)
            heatmap2.draw(0,0);

    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    // can draw all ms data for a single frame? as path?
    void draw(bool bDrawMeanEye, bool bDrawEyes, bool bDrawSaccades, bool bDrawHeatmaps, bool bDrawDifferenceHeatmap, bool bDrawDetail, bool bShowNormalized, bool bDrawClustering, bool bPaused)
    {
        if (bNeedsUpdate) {
            
            assert(subjects.size() == multipleHeatmaps.size());
            
            ofFill();
            for(int condition_i = 0; condition_i < subjects.size(); condition_i++)
            {
                if(condition_i == 1)
                    ofSetColor(0, 255, 255, 100);
                else
                    ofSetColor(255, 255, 0, 100);
                
                // reset heatmap
                multipleHeatmaps[condition_i]->resetMixture();
                
                // add points to heatmap condition for all subjects
                ofVec2f sigma;
                sigma.x = sigmaInPixels;
                sigma.y = sigmaInPixels;
                float weight = 1.0f; // (float)subjects[condition_i].size();
                vector<pkmEyeMovementSubject>::iterator subject = subjects[condition_i].begin();
                while(subject != subjects[condition_i].end())
                {
                    if (subject->bFixation.size() > currentFrame &&
                        subject->bFixation[currentFrame])
                    {
                        
                        if (bDrawMeanEye)
                        {
                            multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularMeanEye[currentFrame].x - offsetX,
                                                                            subject->binocularMeanEye[currentFrame].y - offsetY),
                                                                    sigma,
                                                                    weight);
                            
                            if(!bPaused)
                            {
                                if (currentFrame > 0 &&
                                    ofDist(subject->binocularMeanEye[currentFrame].x,
                                           subject->binocularMeanEye[currentFrame].y,
                                           subject->binocularMeanEye[currentFrame-1].x,
                                           subject->binocularMeanEye[currentFrame-1].y) < (sigmaInPixels * 0.25))
                                    subject->radius += 0.5;
                                else
                                    subject->radius = 5;
                            }
                            
                            
                            if (bDrawSaccades && currentFrame > 1)
                            {
                                ofLine(subject->binocularMeanEye[currentFrame].x - offsetX,
                                       subject->binocularMeanEye[currentFrame].y - offsetY,
                                       subject->binocularMeanEye[currentFrame-1].x - offsetX,
                                       subject->binocularMeanEye[currentFrame-1].y - offsetY);
                                ofLine(subject->binocularMeanEye[currentFrame-2].x - offsetX,
                                       subject->binocularMeanEye[currentFrame-2].y - offsetY,
                                       subject->binocularMeanEye[currentFrame-1].x - offsetX,
                                       subject->binocularMeanEye[currentFrame-1].y - offsetY);
                            }
                            
                            if(bDrawEyes)
                            {
                                ofFill();
                                ofCircle(subject->binocularMeanEye[currentFrame].x - offsetX,
                                         subject->binocularMeanEye[currentFrame].y - offsetY,
                                         subject->radius);
                                ofNoFill();
                                ofCircle(subject->binocularMeanEye[currentFrame].x - offsetX,
                                         subject->binocularMeanEye[currentFrame].y - offsetY,
                                         subject->radius);
                            }
                            
                            if(bDrawDetail)
                            {
                                
                                ofDrawBitmapString(ofToString((int)subject->binocularMeanEye[currentFrame].x - offsetX) + "," + ofToString((int)subject->binocularMeanEye[currentFrame].y - offsetY),
                                                   subject->binocularMeanEye[currentFrame].x - offsetX,
                                                   subject->binocularMeanEye[currentFrame].y - offsetY);
                            }
                            
                        }
                        else
                        {
                            multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularLEye[currentFrame].x - offsetX,
                                                                            subject->binocularLEye[currentFrame].y - offsetY),
                                                                    sigma,
                                                                    weight);
                            
                            ofCircle(subject->binocularLEye[currentFrame].x - offsetX,
                                     subject->binocularLEye[currentFrame].y - offsetY, 0,
                                     5.0);
                            
                            multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularREye[currentFrame].x - offsetX,
                                                                            subject->binocularREye[currentFrame].y - offsetY),
                                                                    sigma,
                                                                    weight);
                            
                            ofCircle(subject->binocularREye[currentFrame].x - offsetX,
                                     subject->binocularREye[currentFrame].y - offsetY, 0,
                                     5.0);
                        }
                    }
                    subject++;
                }
                
            }
            ofSetColor(255);
            ofFill();
            
            if(bDrawHeatmaps)
            {
                
                multipleHeatmaps[0]->buildMixture();
                if(bShowNormalized)
                    multipleHeatmaps[0]->normalizeHeatmap();
                multipleHeatmaps[0]->update(bDrawClustering);
                if(totalClasses > 1)
                {
                    multipleHeatmaps[1]->buildMixture();
                    if(bShowNormalized)
                        multipleHeatmaps[1]->normalizeHeatmap();
                    multipleHeatmaps[1]->update(bDrawClustering);
                    
                    heatmapFBO.begin();
                    glClear(GL_COLOR_BUFFER_BIT);
                    
                    if(bDrawDifferenceHeatmap)
                    {
                        diffShader.begin();
                        diffShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
                        diffShader.setUniform1f("maxValue1", multipleHeatmaps[0]->getMaxValue());

                        diffShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
                        diffShader.setUniform1f("maxValue2", multipleHeatmaps[1]->getMaxValue());
                        
                        multipleHeatmaps[0]->draw();
                        diffShader.end();
                    }
                    else
                    {
                        mixShader.begin();
                        mixShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
                        mixShader.setUniform1f("maxValue1", multipleHeatmaps[0]->getMaxValue());
                        
                        mixShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
                        mixShader.setUniform1f("maxValue2", multipleHeatmaps[1]->getMaxValue());
                        
                        multipleHeatmaps[0]->draw();
                        mixShader.end();
                    }
                
                    heatmapFBO.end();
                }
                else
                {
                    heatmapFBO.begin();
                    glClear(GL_COLOR_BUFFER_BIT);
                    multipleHeatmaps[0]->draw();
                    heatmapFBO.end();
                }
            }
            
            bNeedsUpdate = false;

        }
        
        if(bDrawHeatmaps)
            heatmapFBO.draw(0,0);
        
    }
    //--------------------------------------------------------------
    
private:
    
    //--------------------------------------------------------------
    int                                                 movieWidth, movieHeight;
    float                                               offsetX, offsetY;
    unsigned long                                       currentFrame;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float                                               maxValue1, maxValue2;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float                                               frameRate;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float                                               sigmaInPixels;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    int                                                 numConditions;
    int                                                 numSubjects;
    vector<vector<pkmEyeMovementSubject> >              subjects;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    vector<ofPtr<pkmMixtureOfGaussiansHeatmap> >        multipleHeatmaps;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    pkmDPGMM                                            gmm;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    ofShader                                            mixShader, addShader, subtractShader, diffShader, normShader, jetShader;
    ofFbo                                               heatmap1, heatmap2, heatmapFBO;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    pkmReductionChain                                   reduction;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    int                                                 totalClasses;
    std::map<int, int>                                  classToVectorLUT;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool                                                bMultipleHeatmaps, bNeedsUpdate;
    //--------------------------------------------------------------
    
};