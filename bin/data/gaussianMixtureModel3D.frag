const int MAX_GAUSSIANS = 80;

varying vec2 texcoord;

uniform sampler2DRect image;
uniform float z;
uniform int totalGaussians;
uniform vec3 means[MAX_GAUSSIANS];
uniform vec3 sigmas[MAX_GAUSSIANS];
uniform float weights[MAX_GAUSSIANS];

const float PI = 3.14159265;

void main(){
    float regularizer = 0.0001;
    float dist = 0.0;
    float a = 1.0;
    for(int i = 0; i < totalGaussians; i++)
    {
        float mux = texcoord.x - means[i].x;
        float muy = texcoord.y - means[i].y;
        float muz = z - means[i].z;
        mux *= mux;
        muy *= muy;
        muz *= muz;
        float exponet = mux / (2.0 * sigmas[i].x * sigmas[i].x + regularizer) + 
                        muy / (2.0 * sigmas[i].y * sigmas[i].y + regularizer) + 
                        muz / (2.0 * sigmas[i].z * sigmas[i].z + regularizer);
        dist += weights[i] * a * exp(-exponet);
    }

    gl_FragColor = vec4(dist);

}