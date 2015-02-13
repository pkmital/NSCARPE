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
#include "pkmMatrix.h"
#include "pkmDPGMM.h"

#define MAXSIZE 150
#define STRINGIFY(A) #A

//--------------------------------------------------------------
class pkmMixtureOfGaussians
{
public:
    
    //--------------------------------------------------------------
    pkmMixtureOfGaussians()
    {
        means = (float *)malloc(sizeof(float)*2*MAXSIZE);
        sigmas = (float *)malloc(sizeof(float)*2*MAXSIZE);
        weights = (float *)malloc(sizeof(float)*MAXSIZE);
        size = 0;
        bAllocated = true;
    }
    //--------------------------------------------------------------
    
    
    //--------------------------------------------------------------
    ~pkmMixtureOfGaussians()
    {
        free(means);
        free(sigmas);
        free(weights);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setGaussians(vector<ofVec2f> mean, vector<ofVec2f> sigma, vector<float> weight)
    {
        assert(mean.size() == sigma.size());
        assert(mean.size() == weight.size());
        
        size = MIN(mean.size(), MAXSIZE);
        
        for(int i = 0; i < size; i++)
        {
            means[2*i+0] = mean[i].x;
            means[2*i+1] = mean[i].y;
            sigmas[2*i+0] = sigma[i].x;
            sigmas[2*i+1] = sigma[i].y;
            weights[i] = weight[i];
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float *getMeans()
    {
        return means;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float *getSigmas()
    {
        return sigmas;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float *getWeights()
    {
        return weights;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void clear()
    {
        for(int i = 0; i < size; i++)
        {
            means[2*i] = 0;
            sigmas[2*i] = 0;
            means[2*i+1] = 0;
            sigmas[2*i+1] = 0;
            weights[i] = 0;
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void allocate(int n)
    {
        resize(n);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void resize(int n)
    {
        
        size = n;
        clear();
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void scale(float scalar)
    {
        for(int i = 0; i < size*2; i++)
        {
            means[i] *= scalar;
            sigmas[i] *= scalar;
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    int length()
    {
        return size;
    }
    //--------------------------------------------------------------
    
private:
    
    //--------------------------------------------------------------
    int size;
    
    
    //--------------------------------------------------------------
    float *means;       // 2xN
    float *sigmas;      // 2xN
    float *weights;     // N
    
    
    //--------------------------------------------------------------
    bool bAllocated;
    
};

//--------------------------------------------------------------
class pkmReductionChain
{
public:
    //--------------------------------------------------------------
    pkmReductionChain()
    {
        
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void allocate(int w, int h)
    {
        
        ofLog(OF_LOG_NOTICE, "Loading Reduction Shader...");
        reductionShader.load("data/maxreduction");
        
        width = w;
        height = h;
        
        ofFbo::Settings settings;
        settings.minFilter = GL_NEAREST;
        settings.maxFilter = GL_NEAREST;
        settings.internalformat = GL_RGBA32F_ARB;
        settings.numSamples = 0;
        settings.useDepth = false;
        settings.useStencil = false;
        
        int i = 0;
        //cout << "Reducing: " << w << " " << h << endl;
        while (w != 1 || h != 1) {
            w = max(w/2,1);
            h = max(h/2,1);
            //cout << "w: " << w << " h: " << h << endl;
            settings.width = w;
            settings.height = h;
            reductionChain.push_back(ofFbo());
            reductionChain.back().allocate(settings);
            
            reductionChain.back().begin();
            glClear( GL_COLOR_BUFFER_BIT );
            reductionChain.back().end();
            i++;
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    double getEntropyCPU(ofTexture& tex)
    {
        ofFloatPixels pixels;
        tex.readToPixels(pixels);
        double entropy = 0.0f;
        //cout << "w: " << tex.getWidth() << " h: " << tex.getHeight() << endl;
        for (int i = 0; i < tex.getWidth() * tex.getHeight(); i++) {
            entropy += pixels[4*i] * logf(pixels[4*i]);
        }
        return entropy;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    double getMaximumCPU(ofTexture& tex)
    {
        ofFloatPixels pixels;
        tex.readToPixels(pixels);
        double maxValue = 0.0f;
//        cout << "w: " << tex.getWidth() << " h: " << tex.getHeight() << endl;
        for (int i = 0; i < tex.getWidth() * tex.getHeight(); i++) {
            if(maxValue < pixels[4*i])
                maxValue = pixels[4*i];
        }
        return maxValue;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    float getMaximum(ofTexture& tex)
    {
        ofFloatPixels pixels;
        float maxValue = 0.0f;
        
        reductionChain[0].begin();
        reductionShader.begin();
        reductionShader.setUniformTexture("image", tex, 0);
        tex.draw(0, 0);
        reductionShader.end();
        reductionChain[0].end();
        
        int i = 1;
        while (i < reductionChain.size()) {
            reductionChain[i].begin();
            reductionShader.begin();
            reductionShader.setUniformTexture("image", reductionChain[i-1].getTextureReference(), 0);
            reductionChain[i-1].draw(0, 0, reductionChain[i].getWidth(), reductionChain[i].getHeight());
            reductionShader.end();
            reductionChain[i].end();
            i++;
        }
        
        reductionChain.back().readToPixels(pixels);
        
        float *pix = pixels.getPixels();
        maxValue = pix[0];
        for (int i = 0; i < pixels.size(); i++)
        {
            //cout << "i: " << i << " " << pix[i] << ", ";
            if (pix[i] > maxValue) {
                maxValue = pix[i];
            }
        }
        //cout << endl;
        
        return maxValue;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void drawReduction()
    {
        ofPushMatrix();
        for (int i = 0; i < reductionChain.size(); i++) {
            reductionChain[i].draw(0, 0);
            ofTranslate(reductionChain[i].getWidth(), 0);
        }
        ofPopMatrix();
    }
    //--------------------------------------------------------------
    
private:
    //--------------------------------------------------------------
    vector<ofFbo>           reductionChain;
    ofShader                reductionShader;
    int                     width, height;
    //--------------------------------------------------------------
};

//--------------------------------------------------------------
class pkmMixtureOfGaussiansHeatmap
{
public:
    
    //--------------------------------------------------------------
    enum colormode {
        heatmap_jet,
        heatmap_hot,
        heatmap_cool,
        heatmap_gray
    };
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    pkmMixtureOfGaussiansHeatmap()
    {
        bAllocated = false;
        bNeedsUpdate = true;
        bNormalize = true;
        ofLog(OF_LOG_NOTICE, "Loading Mixture Model Shader...");
        posteriorMap.load("data/gaussianMixtureModel");
        setColorMap(heatmap_jet);
        maxValue = 1.0f;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void allocate(int w, int h, float s = 1.0)
    {
        scale = s;
        width = w / s;
        height = h / s;
        
        ofFbo::Settings settings;
        settings.minFilter = GL_NEAREST;
        settings.maxFilter = GL_NEAREST;
        settings.width = width;
        settings.height = height;
        settings.internalformat = GL_RGBA32F_ARB;
        settings.numSamples = 0;
        settings.useDepth = false;
        settings.useStencil = false;
        
        posteriorMapFBO.allocate(settings);
        posteriorMap2FBO.allocate(settings);
        
        settings.width = w;
        settings.height = h;
        finalFBO.allocate(settings);
        
        posteriorMapFBO.begin();
        glClear( GL_COLOR_BUFFER_BIT );
        posteriorMapFBO.end();
        
        posteriorMap2FBO.begin();
        glClear( GL_COLOR_BUFFER_BIT );
        posteriorMap2FBO.end();
        
        finalFBO.begin();
        glClear( GL_COLOR_BUFFER_BIT );
        finalFBO.end();
        
        reduction.allocate(width, height);
        
        resetMixture();
        
        bAllocated = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setColorMap(colormode c)
    {
        if (c == heatmap_jet) {
            ofLog(OF_LOG_NOTICE, "Loading Jetmap Shader");
            colormap.load("data/jetmap");
        }
        else if (c == heatmap_hot) {
            colormap.load("data/hotmap");
            ofLog(OF_LOG_NOTICE, "Loading Hot Shader");
        }
        else if (c == heatmap_cool) {
            colormap.load("data/coolmap");
            ofLog(OF_LOG_NOTICE, "Loading Cool Shader");
        }
        else if (c == heatmap_gray) {
            colormap.load("data/graymap");
            ofLog(OF_LOG_NOTICE, "Loading Gray Shader");
        }
        
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void toggleNormalization()
    {
        bNormalize = !bNormalize;
        
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void addPoint(ofVec2f mean, ofVec2f sigma, float weight)
    {
        means.push_back(mean);
        sigmas.push_back(sigma);
        weights.push_back(weight);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void addPointForFrame(int frame, ofVec2f mean, ofVec2f sigma, float weight)
    {
        means.push_back(mean);
        sigmas.push_back(sigma);
        weights.push_back(weight);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setMixture(pkmMixtureOfGaussians mix)
    {
        mixture = mix;
        
        mixture.scale(scale);
        
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setMixture(vector<ofVec2f> means,
                    vector<ofVec2f> sigmas,
                    vector<float> weights)
    {
        mixture.clear();
        
        float heatmapScalar = 0.0;
        int totalGaussians = means.size();
        //        for (int i = 0; i < totalGaussians; i++) {
        //            float a = 1.0 / (2.0 * PI * sigmas[i].x * sigmas[i].y);
        //            heatmapScalar = max(heatmapScalar,(a * weights[i] * expf(0.0)));
        //        }
        
        mixture.setGaussians(means, sigmas, weights);
        
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void resetMixture()
    {
        mixture.clear();
        means.resize(0);
        sigmas.resize(0);
        weights.resize(0);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void buildMixture()
    {
        int totalGaussians = means.size();
        if(totalGaussians > 0)
        {
            mixture.setGaussians(means, sigmas, weights);
            
            bNeedsUpdate = true;
        }
        else
        {
            ofLog(OF_LOG_WARNING, "No gaussians to draw!");
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void setRandomMixture(int totalGaussians = MAXSIZE)
    {
        mixture.clear();
        vector<ofVec2f> means;
        vector<ofVec2f> sigmas;
        vector<float> weights;
        
        for (int i = 0; i < totalGaussians; i++) {
            means.push_back(ofVec2f(rand() % width, rand() % height));
            float s = max(50,rand() % height);
            sigmas.push_back(ofVec2f(s, s));
            weights.push_back(1.0 / totalGaussians);
        }
        
        float heatmapScalar = 0.0;
        for (int i = 0; i < totalGaussians; i++) {
            float a = 1.0 / (2.0 * PI * sigmas[i].x * sigmas[i].y);
            heatmapScalar = max(heatmapScalar,(a * weights[i] * expf(0.0)));
        }
        
        mixture.setGaussians(means, sigmas, weights);
        
        
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void update(bool bCluster)
    {
        if(bNeedsUpdate)
        {
            bNeedsUpdate = false;
            
            posteriorMapFBO.begin();
            {
                glClear( GL_COLOR_BUFFER_BIT );
                
                posteriorMap.begin();
                {
                    int totalGaussians = means.size();
                    
                    if(bCluster)
                    {
                        size_t feature_length = 2;
                        pkm::Mat input(mixture.length(), feature_length, mixture.getMeans()), means, covars, sigmas, weights;
                        pkmDPGMM::cluster(input, means, covars, weights, feature_length);
                        for(int i = 0; i <= means.rows; i++)
                        {
                            for(int f = 0; f < feature_length; f++)
                                sigmas.push_back(covars.row(2*i+f)[f]);
                        }
                        sigmas.sqrt();
                        
//                        cout << "input: " <<endl;
//                        input.print();
//                        cout << "means: " <<endl;
//                        means.print();
//                        cout << "sigmas: " <<endl;
//                        sigmas.print();
//                        cout << "weights: " <<endl;
//                        weights.print();
                        
                        posteriorMap.setUniform2fv("means", means.data, means.rows);
                        posteriorMap.setUniform2fv("sigmas", sigmas.data, means.rows);
                        posteriorMap.setUniform1fv("weights", weights.data, means.rows);
                        posteriorMap.setUniform1i("totalGaussians", MIN(totalGaussians, means.rows));
                    }
                    else
                    {
                        posteriorMap.setUniform2fv("means", mixture.getMeans(), MAXSIZE);
                        posteriorMap.setUniform2fv("sigmas", mixture.getSigmas(), MAXSIZE);
                        posteriorMap.setUniform1fv("weights", mixture.getWeights(), MAXSIZE);
                        posteriorMap.setUniform1i("totalGaussians", MIN(totalGaussians, MAXSIZE));
                    }
                    //                posteriorMap.setUniform1f("scalar", 1.0f);
                    posteriorMap.setUniformTexture("image", posteriorMap2FBO.getTextureReference(), 0);
                    
                    posteriorMap2FBO.draw(0, 0, width, height);
                }
                posteriorMap.end();
            }
            posteriorMapFBO.end();
            
            finalFBO.begin();
            glClear(GL_COLOR_BUFFER_BIT);
            colormap.begin();
            colormap.setUniformTexture("image", posteriorMapFBO.getTextureReference(), 0);
            colormap.setUniform1f("maxValue", maxValue);
            posteriorMapFBO.draw(0, 0, width*scale, height*scale);
            colormap.end();
            finalFBO.end();
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void normalizeHeatmap()
    {
        if(bNormalize)
        {
            maxValue = reduction.getMaximumCPU(posteriorMapFBO.getTextureReference());
//            cout << "max value: " << maxValue << endl;
        }
        else
        {
            maxValue = 1.0f;
        }
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void drawUnColored()
    {
        posteriorMapFBO.draw(0, 0, width, height);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void draw()
    {
        finalFBO.draw(0, 0, width, height);
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    ofTexture & getTextureReferenceOfUnColored()
    {
        return posteriorMapFBO.getTextureReference();
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    ofTexture & getTextureReference()
    {
        return finalFBO.getTextureReference();
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    void clear()
    {
        resetMixture();
        bNeedsUpdate = true;
    }
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    double getMaxValue()
    {
        return maxValue;
    }
    //--------------------------------------------------------------
    
private:
    //--------------------------------------------------------------
    ofShader                colormap;
    ofShader                posteriorMap;
    pkmMixtureOfGaussians   mixture;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    vector<ofVec2f>         means;
    vector<ofVec2f>         sigmas;
    vector<float>           weights;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    ofFbo                   posteriorMap2FBO;
    ofFbo                   posteriorMapFBO;
    ofFbo                   finalFBO;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    pkmReductionChain       reduction;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    bool                    bAllocated, bNeedsUpdate, bNormalize;
    //--------------------------------------------------------------
    
    //--------------------------------------------------------------
    int                     width, height;
    float                   scale;
    double                  maxValue;
    //--------------------------------------------------------------
    
};












