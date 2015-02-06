const int MAX_GAUSSIANS = 50;
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
    float a = 1.0 / 2.502;
    for(int i = 0; i < totalGaussians; i++)
    {
        float mux = texcoord.x - means[i].x;
        float muy = texcoord.y - means[i].y;
        float exponet = (mux * mux) / (2.0 * sigmas[i].x * sigmas[i].x + regularizer) +
        (muy * muy) / (2.0 * sigmas[i].y * sigmas[i].y + regularizer);
        exponet *= exponet;
        dist += weights[i] * a * exp(-exponet/2.0);
    }
    gl_FragColor = vec4(1.0);
}