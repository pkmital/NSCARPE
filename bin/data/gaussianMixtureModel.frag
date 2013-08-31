const int MAX_GAUSSIANS = 80;
uniform int totalGaussians;
uniform sampler2DRect image;
//
varying vec2 texcoord;
//
uniform vec2 means[MAX_GAUSSIANS]; // x,y
uniform vec2 sigmas[MAX_GAUSSIANS]; // x,y
uniform float weights[MAX_GAUSSIANS];
//uniform float heatmapScalar;

const float PI = 3.14159265;

void main(){
    float regularizer = 0.0001;
    float dist = 0.0;
    float a = 1.0;// / float(totalGaussians);// / (2.0 * PI * sigmas[i].x * sigmas[i].y + regularizer);
    for(int i = 0; i < totalGaussians; i++)
    {
        float mux = texcoord.x - means[i].x;
        float muy = texcoord.y - means[i].y;
        mux *= mux;
        muy *= muy;
        float exponet = mux / (2.0 * sigmas[i].x * sigmas[i].x + regularizer) + 
                        muy / (2.0 * sigmas[i].y * sigmas[i].y + regularizer);
        dist += weights[i] * a * exp(-exponet);
    }

    gl_FragColor = vec4(dist);

}