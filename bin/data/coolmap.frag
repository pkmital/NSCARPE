uniform sampler2DRect image;
uniform float maxValue;
varying vec2 texcoordM;

void main() {
    float max = 1.0;
    float max3 = 1.0 / 3.0;
    float dist = texture2DRect(image, texcoordM).r / maxValue;

    if(dist <= 0.0)
    {
        gl_FragColor.rgb = vec3(0.0,0.0,0.0);
    }
    else if(dist <= (2.0 * max3))
    {
    	gl_FragColor.r = 0.0;
        gl_FragColor.g = (dist / max) * (dist / (2.0 * max3));
        gl_FragColor.b = (1.0 - gl_FragColor.r) * (dist / (2.0 * max3));
    }
    else
    {
        gl_FragColor.r = 0.0;
        gl_FragColor.g = dist / max;
        gl_FragColor.b = 1.0 - gl_FragColor.r;
    }
    gl_FragColor.a = dist;
}
