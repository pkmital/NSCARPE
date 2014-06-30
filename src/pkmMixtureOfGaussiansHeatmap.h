#pragma once

#include "ofMain.h"

#define MAXSIZE 100
#define STRINGIFY(A) #A

class pkmMixtureOfGaussians
{
public:
    
    pkmMixtureOfGaussians()
    {
        means = (float *)malloc(sizeof(float)*2*MAXSIZE);
        sigmas = (float *)malloc(sizeof(float)*2*MAXSIZE);
        weights = (float *)malloc(sizeof(float)*MAXSIZE);
        size = 0;
        bAllocated = true;
    }
    ~pkmMixtureOfGaussians()
    {
        free(means);
        free(sigmas);
        free(weights);
    }
    
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
    
    float *getMeans()
    {
        return means;
    }
    
    float *getSigmas()
    {
        return sigmas;
    }
    
    float *getWeights()
    {
        return weights;
    }
    
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
    
    void allocate(int n)
    {
        resize(n);
    }
    
    void resize(int n)
    {
        
        size = n;
        clear();
    }
    
    void scale(float scalar)
    {
        for(int i = 0; i < size*2; i++)
        {
            means[i] *= scalar;
            sigmas[i] *= scalar;
        }
    }
    
    int length()
    {
        return size;
    }
    
private:
    
    int size;
    float *means;       // 2xN
    float *sigmas;      // 2xN
    float *weights;     // N
    
    bool bAllocated;
    
};

class pkmReductionChain
{
public:
    pkmReductionChain()
    {
    }
    
    void allocate(int w, int h)
    {
        
        ofLog(OF_LOG_NOTICE, "Loading Reduction Shader...");
        reductionShader.load(ofToDataPath("maxreduction", true));
        
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
    
    double getMaximumCPU(ofTexture& tex)
    {
        ofFloatPixels pixels;
        tex.readToPixels(pixels);
        double maxValue = 0.0f;
        //cout << "w: " << tex.getWidth() << " h: " << tex.getHeight() << endl;
        for (int i = 0; i < tex.getWidth() * tex.getHeight(); i++) {
            if(maxValue < pixels[4*i + 3])
                maxValue = pixels[4*i + 3];
        }
        return maxValue;
    }
    
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
    
    
    void drawReduction()
    {
        ofPushMatrix();
        for (int i = 0; i < reductionChain.size(); i++) {
            reductionChain[i].draw(0, 0);
            ofTranslate(reductionChain[i].getWidth(), 0);
        }
        ofPopMatrix();
    }
    
private:
    vector<ofFbo>           reductionChain;
    ofShader                reductionShader;
    int                     width, height;
};

class pkmMixtureOfGaussiansHeatmap
{
public:
    
    enum colormode {
        heatmap_jet,
        heatmap_hot,
        heatmap_cool,
        heatmap_gray
    };
    
    pkmMixtureOfGaussiansHeatmap()
    {
        bAllocated = false;
        bNeedsUpdate = true;
        bNormalize = true;
        ofLog(OF_LOG_NOTICE, "Loading Mixture Model Shader...");
        posteriorMap.load(ofToDataPath("gaussianMixtureModel", true));
        setColorMap(heatmap_jet);
        maxValue = 1.0f;
    }
    
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
    
    void setColorMap(colormode c)
    {
        if (c == heatmap_jet) {
            ofLog(OF_LOG_NOTICE, "Loading Jetmap Shader");
            colormap.load(ofToDataPath("jetmap", true));
        }
        else if (c == heatmap_hot) {
            colormap.load(ofToDataPath("hotmap", true));
            ofLog(OF_LOG_NOTICE, "Loading Hot Shader");
        }
        else if (c == heatmap_cool) {
            colormap.load(ofToDataPath("coolmap", true));
            ofLog(OF_LOG_NOTICE, "Loading Cool Shader");
        }
        else if (c == heatmap_gray) {
            colormap.load(ofToDataPath("graymap", true));
            ofLog(OF_LOG_NOTICE, "Loading Gray Shader");
        }
        
        bNeedsUpdate = true;
    }
    
    void toggleNormalization()
    {
        bNormalize = !bNormalize;
        
        bNeedsUpdate = true;
    }
    
    void setMixture(pkmMixtureOfGaussians mix)
    {
        mixture = mix;
        
        mixture.scale(scale);
        
        //        float heatmapScalar = 0.0;
        //        for (int i = 0; i < mixture.length(); i++) {
        //            float a = 1.0 / (2.0 * PI * mixture.getSigmas()[2*i+0] * mixture.getSigmas()[2*i+0]);
        //            heatmapScalar = max(heatmapScalar,(a * mixture.getWeights()[i] * expf(0.0)));
        //        }
        
        posteriorMapFBO.begin();
        {
            glClear( GL_COLOR_BUFFER_BIT );
            
            posteriorMap.begin();
            {
                posteriorMap.setUniform2fv("means", mixture.getMeans(), MAXSIZE);
                posteriorMap.setUniform2fv("sigmas", mixture.getSigmas(), MAXSIZE);
                posteriorMap.setUniform1fv("weights", mixture.getWeights(), MAXSIZE);
                posteriorMap.setUniform1i("totalGaussians", MIN(MAXSIZE,mixture.length()));
                //                posteriorMap.setUniform1f("scalar", 1.0);
                posteriorMap.setUniformTexture("image", posteriorMap2FBO.getTextureReference(), 0);
                
                posteriorMap2FBO.draw(0, 0, width, height);
            }
            posteriorMap.end();
        }
        posteriorMapFBO.end();
        
        normalizeHeatmap();
        
        bNeedsUpdate = true;
    }
    
    void resetMixture()
    {
        mixture.clear();
        means.resize(0);
        sigmas.resize(0);
        weights.resize(0);
    }
    
    void addPoint(ofVec2f mean, ofVec2f sigma, float weight)
    {
        means.push_back(mean);
        sigmas.push_back(sigma);
        weights.push_back(weight);
    }
    
    void buildMixture()
    {
        int totalGaussians = means.size();
        //cout << "total: " << totalGaussians << endl;
        if(totalGaussians > 0)
        {
            mixture.setGaussians(means, sigmas, weights);
            
            //            for (int i = 0; i < totalGaussians; i++) {
            //                cout << mixture.getMeans()[i*2+0] << "," <<
            //                        mixture.getMeans()[i*2+1] << " " <<
            //                        mixture.getSigmas()[i*2+0] << "," <<
            //                        mixture.getSigmas()[i*2+1] << " " <<
            //                        mixture.getWeights()[i] << endl;
            //            }
            
            posteriorMap2FBO.begin();
            glClear( GL_COLOR_BUFFER_BIT );
            posteriorMap2FBO.end();
            
            posteriorMapFBO.begin();
            glClear( GL_COLOR_BUFFER_BIT );
            
            posteriorMap.begin();
            posteriorMap.setUniform2fv("means", mixture.getMeans(), MAXSIZE);
            posteriorMap.setUniform2fv("sigmas", mixture.getSigmas(), MAXSIZE);
            posteriorMap.setUniform1fv("weights", mixture.getWeights(), MAXSIZE);
            posteriorMap.setUniform1i("totalGaussians", MIN(totalGaussians, MAXSIZE));
            posteriorMap.setUniformTexture("image", posteriorMap2FBO.getTextureReference(), 0);
            posteriorMap2FBO.draw(0, 0, width, height);
            posteriorMap.end();
            
            posteriorMapFBO.end();
            
            normalizeHeatmap();
            bNeedsUpdate = true;
        }
        else
        {
            ofLog(OF_LOG_WARNING, "No gaussians to draw!");
        }
    }
    
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
        
        posteriorMapFBO.begin();
        {
            glClear( GL_COLOR_BUFFER_BIT );
            
            posteriorMap.begin();
            {
                posteriorMap.setUniform2fv("means", mixture.getMeans(), MAXSIZE);
                posteriorMap.setUniform2fv("sigmas", mixture.getSigmas(), MAXSIZE);
                posteriorMap.setUniform1fv("weights", mixture.getWeights(), MAXSIZE);
                posteriorMap.setUniform1i("totalGaussians", MIN(totalGaussians, MAXSIZE));
                //                posteriorMap.setUniform1f("scalar", 1.0f);
                posteriorMap.setUniformTexture("image", posteriorMap2FBO.getTextureReference(), 0);
                
                posteriorMap2FBO.draw(0, 0, width, height);
            }
            posteriorMap.end();
        }
        posteriorMapFBO.end();
        
        normalizeHeatmap();
        
        bNeedsUpdate = true;
        
    }
    
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
        
        posteriorMapFBO.begin();
        {
            glClear( GL_COLOR_BUFFER_BIT );
            
            posteriorMap.begin();
            {
                posteriorMap.setUniform2fv("means", mixture.getMeans(), totalGaussians);
                posteriorMap.setUniform2fv("sigmas", mixture.getSigmas(), totalGaussians);
                posteriorMap.setUniform1fv("weights", mixture.getWeights(), totalGaussians);
                posteriorMap.setUniform1i("totalGaussians", MIN(totalGaussians, MAXSIZE));
                posteriorMap.setUniformTexture("image", posteriorMap2FBO.getTextureReference(), 0);
                
                posteriorMap2FBO.draw(0, 0, width, height);
            }
            posteriorMap.end();
        }
        posteriorMapFBO.end();
        
        normalizeHeatmap();
        
        bNeedsUpdate = true;
    }
    
    
    void update()
    {
        if(bNeedsUpdate)
        {
            bNeedsUpdate = false;
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
    
    void normalizeHeatmap()
    {
        if(bNormalize)
        {
            maxValue = reduction.getMaximumCPU(posteriorMapFBO.getTextureReference());
        }
        else
        {
            maxValue = 1.0f;
        }
    }
    
    void drawUnColored()
    {
        posteriorMapFBO.draw(0, 0, width, height);
    }
    
    void draw()
    {
        finalFBO.draw(0, 0, width, height);
    }
    
    ofTexture & getTextureReferenceOfUnColored()
    {
        return posteriorMapFBO.getTextureReference();
    }
    
    ofTexture & getTextureReference()
    {
        return finalFBO.getTextureReference();
    }
    
    void clear()
    {
        resetMixture();
        bNeedsUpdate = true;
    }
    
private:
    ofShader                colormap;
    ofShader                posteriorMap;
    pkmMixtureOfGaussians   mixture;
    
    vector<ofVec2f>         means;
    vector<ofVec2f>         sigmas;
    vector<float>           weights;
    
    ofFbo                   posteriorMap2FBO;
    ofFbo                   posteriorMapFBO;
    ofFbo                   finalFBO;
    
    pkmReductionChain       reduction;
    
    bool                    bAllocated, bNeedsUpdate, bNormalize;
    
    int                     width, height;
    float                   scale;
    double                  maxValue;
    
};












