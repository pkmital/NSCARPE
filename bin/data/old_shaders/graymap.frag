uniform sampler2DRect image;
uniform float maxValue;
varying vec2 texcoordM;
const float PI = 3.14159265;
const float c1 = 0.1;
const float colorMax = 1.0;

void main() {
    float dist = texture2DRect(image, texcoordM).r / maxValue;
    gl_FragColor = vec4(dist, dist, dist, dist);
}
