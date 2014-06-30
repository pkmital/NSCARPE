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

class pkmEyeMovementSubject {
public:
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
    //    vector<int>         classification;
    vector<bool>        bFixation;
    //    vector<int>         frameNumber;
    map<int,int>        msLookUp;
    vector<ofVec2f>     previousEye;
    string              subjectName;
};

class pkmEyeMovementCollection {
    
public:
    pkmEyeMovementCollection()
    {
        totalClasses            = 0;
        offsetX                 = 0;
        offsetY                 = 0;
        currentFrame            = 0;
        numSubjects             = 0;
        numConditions           = 0;
        bMultipleHeatmaps       = false;
        frameRate               = 0;
        sigmaInPixels           = 0;
        maxValue1 = maxValue2   = 1.0;
    }
    
    void allocateHeatmap(int movieWidth, int movieHeight, int offsetX, int offsetY)
    {
        this->movieWidth = movieWidth;
        this->movieHeight = movieHeight;
        this->offsetX = offsetX;
        this->offsetY = offsetY;
        
        reduction.allocate(movieWidth, movieHeight);
        
        ofLog(OF_LOG_NOTICE, "Loading FBO");
        
        ofFbo::Settings settings;
        settings.minFilter = GL_NEAREST;
        settings.maxFilter = GL_NEAREST;
        settings.width = movieWidth;
        settings.height = movieHeight;
        settings.internalformat = GL_RGBA32F_ARB;
        settings.numSamples = 0;
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
        mixShader.load(ofToDataPath("mix", true));
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
    }
    
    void loadArringtonFiles(string path)
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
        h->setColorMap((pkmMixtureOfGaussiansHeatmap::colormode)0);
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
                        subjects[conditionNumber][subject_i].msLookUp[msCounter] = subjects[conditionNumber][subject_i].binocularLEye.size();
                    }
                }
                
                frameCounter++;
            }
            catch(...)
            {
                
            }
        }
    }
    
    void loadFiles(string path,                         // location of files
                   //string token = "",                   // token of the files in the directory to load
                   bool bLoadBinocular = true,          // format of eye-data has twice as many columns for the second eye
                   int conditionNumber = 0)     // (for multiple heatmaps)
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
        
        // create new vector of subjects for condition
        vector<pkmEyeMovementSubject> a;
        subjects.push_back(a);
        subjects[conditionNumber].resize(numSubjects);
        
        // create new heatmap for condition
        ofPtr<pkmMixtureOfGaussiansHeatmap> h(new pkmMixtureOfGaussiansHeatmap());
        h->allocate(movieWidth, movieHeight);
        h->setColorMap((pkmMixtureOfGaussiansHeatmap::colormode)(conditionNumber+1));
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
                    string one, two, three, four, five, six, seven, eight, nine, ten;
                    float x, y, x2, y2, dil, dil2;
                    int left_event, right_event;
                    if(bLoadBinocular)
                    {
                        // [ms] [frame] [left_x] [left_y] [left_dil] [left_event] [right_x] [right_y] [right_dil] [right_event]
                        eyeFile >> msCounter >> one >> two >> three >> four >> five >> six >> seven >> eight >> nine;
                        
                        istringstream instr(two);	instr >> x;             instr.clear();
                        instr.str(three);           instr >> y;             instr.clear();
                        instr.str(four);            instr >> dil;           instr.clear();
                        instr.str(five);            instr >> left_event; 	instr.clear();
                        instr.str(six);				instr >> x2; 			instr.clear();
                        instr.str(seven);			instr >> y2; 			instr.clear();
                        instr.str(eight);			instr >> dil2; 			instr.clear();
                        instr.str(nine);			instr >> right_event;   instr.clear();
                        
                        //                    subjects[conditionNumber][i].classification.push_back(conditionNumber);
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
                        subjects[conditionNumber][subject_i].msLookUp[msCounter] = subjects[conditionNumber][subject_i].binocularLEye.size();
                        
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
			
            // close the file after reading it
			eyeFile.close();
        }
        ofLog(OF_LOG_NOTICE, "Loaded %d files for condition %d", numSubjects, conditionNumber);
    }
    
    void setSigma(float varianceInPixels)
    {
        sigmaInPixels = varianceInPixels;
    }
    
    
    void setFrameRate(float framesPerSecond)
    {
        frameRate = framesPerSecond;
    }
    
    void setTime(float seconds)
    {
        setFrame(seconds * frameRate);
    }
    
    void setFrame(unsigned long frame)
    {
        currentFrame = frame;
    }
    
    // can draw all ms data for a single frame? as path?
    void draw(int numFrames, bool bDrawMeanEye = true)
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
            float weight = 1.0f / (float)subjects[condition_i].size();
            vector<pkmEyeMovementSubject>::iterator subject = subjects[condition_i].begin();
            while(subject != subjects[condition_i].end())
            {
                for(int frame_i = 0; frame_i < numFrames; frame_i++)
                {
                    if (subject->msLookUp.find(currentFrame+frame_i) != subject->msLookUp.end() &&
                        subject->bFixation[subject->msLookUp[currentFrame+frame_i]])
                    {
                        unsigned long subjectFrameIdx = subject->msLookUp[currentFrame+frame_i];
                        
                        if (bDrawMeanEye)
                        {
                            multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                                                            subject->binocularMeanEye[subjectFrameIdx].y - offsetY),
                                                                    sigma,
                                                                    weight);
                            ofCircle(subject->binocularMeanEye[subjectFrameIdx].x - offsetX,
                                     subject->binocularMeanEye[subjectFrameIdx].y - offsetY,
                                     8.0);
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
        
#if(0)
        multipleHeatmaps[0]->buildMixture();
        multipleHeatmaps[0]->update();
        multipleHeatmaps[1]->buildMixture();
        multipleHeatmaps[1]->update();
        
        heatmapFBO.begin();
        glClear(GL_COLOR_BUFFER_BIT);
        multipleHeatmaps[0]->draw();
        multipleHeatmaps[1]->draw();
        heatmapFBO.end();
        heatmapFBO.draw(0, 0);
        
#else
        
        multipleHeatmaps[0]->buildMixture();
        multipleHeatmaps[0]->update();
        multipleHeatmaps[1]->buildMixture();
        multipleHeatmaps[1]->update();
        
        // first subtract and take abs, then find max of this map
        // then do colormap difference using this maxvalue
        heatmap1.begin();
        glClear(GL_COLOR_BUFFER_BIT);
        subtractShader.begin();
        subtractShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
        subtractShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
        multipleHeatmaps[0]->draw();
        subtractShader.end();
        heatmap1.end();
        
        float maxValue1 = reduction.getMaximumCPU(heatmap1.getTextureReference());
        float maxValue2 = reduction.getMaximumCPU(heatmap2.getTextureReference());
        heatmap2.begin();
        glClear(GL_COLOR_BUFFER_BIT);
        diffShader.begin();
        diffShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
        diffShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
        diffShader.setUniform1f("maxValue1", maxValue1);
        diffShader.setUniform1f("maxValue2", maxValue2);
        multipleHeatmaps[0]->draw();
        diffShader.end();
        heatmap2.end();
        
        heatmap2.draw(0,0);
        
#endif
    }
    
    // can draw all ms data for a single frame? as path?
    void draw(bool bDrawMeanEye = true)
    {
        assert(subjects.size() == multipleHeatmaps.size());
        
        ofNoFill();
        for(int condition_i = 0; condition_i < subjects.size(); condition_i++)
        {
            if(condition_i == 1)
                ofSetColor(0, 255, 255);
            else
                ofSetColor(255, 255, 0);
            
            // reset heatmap
            multipleHeatmaps[condition_i]->resetMixture();
            
            // add points to heatmap condition for all subjects
            ofVec2f sigma;
            sigma.x = sigmaInPixels;
            sigma.y = sigmaInPixels;
            float weight = 1.0f / (float)subjects[condition_i].size();
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
                        ofCircle(subject->binocularMeanEye[currentFrame].x - offsetX,
                                 subject->binocularMeanEye[currentFrame].y - offsetY,
                                 4.0);
                    }
                    else
                    {
                        multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularLEye[currentFrame].x - offsetX,
                                                                        subject->binocularLEye[currentFrame].y - offsetY),
                                                                sigma,
                                                                weight);
                        
                        ofCircle(subject->binocularLEye[currentFrame].x - offsetX,
                                 subject->binocularLEye[currentFrame].y - offsetY,
                                 4.0);
                        
                        multipleHeatmaps[condition_i]->addPoint(ofVec2f(subject->binocularREye[currentFrame].x - offsetX,
                                                                        subject->binocularREye[currentFrame].y - offsetY),
                                                                sigma,
                                                                weight);
                        
                        ofCircle(subject->binocularREye[currentFrame].x - offsetX,
                                 subject->binocularREye[currentFrame].y - offsetY,
                                 4.0);
                    }
                }
                subject++;
            }
            
        }
        ofSetColor(255);
        ofFill();
        
#if(0)
        multipleHeatmaps[0]->buildMixture();
        multipleHeatmaps[0]->update();
        
        heatmapFBO.begin();
        glClear(GL_COLOR_BUFFER_BIT);
        multipleHeatmaps[0]->draw();
        heatmapFBO.end();
        heatmapFBO.draw(0, 0);
        
        //
        //        multipleHeatmaps[0]->buildMixture();
        //        multipleHeatmaps[0]->update();
        //        multipleHeatmaps[1]->buildMixture();
        //        multipleHeatmaps[1]->update();
        //
        //        heatmapFBO.begin();
        //        glClear(GL_COLOR_BUFFER_BIT);
        //        multipleHeatmaps[0]->draw();
        //        multipleHeatmaps[1]->draw();
        //        heatmapFBO.end();
        //        heatmapFBO.draw(0, 0);
        
#else
        
        multipleHeatmaps[0]->buildMixture();
        multipleHeatmaps[0]->update();
        multipleHeatmaps[1]->buildMixture();
        multipleHeatmaps[1]->update();
        
        //        // first subtract and take abs, then find max of this map
        //        // then do colormap difference using this maxvalue
        //        heatmap1.begin();
        //        glClear(GL_COLOR_BUFFER_BIT);
        //        subtractShader.begin();
        //        subtractShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
        //        subtractShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
        //        multipleHeatmaps[0]->draw();
        //        subtractShader.end();
        //        heatmap1.end();
        
        maxValue1 = reduction.getMaximumCPU(multipleHeatmaps[0]->getTextureReference());
        maxValue2 = reduction.getMaximumCPU(multipleHeatmaps[1]->getTextureReference());
        heatmap2.begin();
        glClear(GL_COLOR_BUFFER_BIT);
        diffShader.begin();
        diffShader.setUniformTexture("img1", multipleHeatmaps[0]->getTextureReference(), 0);
        diffShader.setUniformTexture("img2", multipleHeatmaps[1]->getTextureReference(), 1);
        diffShader.setUniform1f("maxValue1", maxValue1);
        diffShader.setUniform1f("maxValue2", maxValue2);
        multipleHeatmaps[0]->draw();
        diffShader.end();
        heatmap2.end();
        
        heatmap2.draw(0,0);
        
#endif
    }
    
private:
    int                                                 movieWidth, movieHeight;
    float                                               offsetX, offsetY;
    unsigned long                                       currentFrame;
    
    float                                               maxValue1, maxValue2;
    
    float                                               frameRate;
    
    float                                               sigmaInPixels;
    
    int                                                 numConditions;
    int                                                 numSubjects;
    vector<vector<pkmEyeMovementSubject> >              subjects;
    
    vector<ofPtr<pkmMixtureOfGaussiansHeatmap> >        multipleHeatmaps;

    ofShader                                            mixShader, addShader, subtractShader, diffShader, normShader, jetShader;
    ofFbo                                               heatmap1, heatmap2, heatmapFBO;
    pkmReductionChain                                   reduction;
    
    int                                                 totalClasses;
    std::map<int, int>                                  classToVectorLUT;
    
    bool                                                bMultipleHeatmaps;
    
};