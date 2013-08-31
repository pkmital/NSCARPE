uniform sampler2DRect image;
uniform float maxValue;
varying vec2 texcoordM;

void main() {
    float max3 = 1.0 / 3.0;
    float dist = min(1.0, texture2DRect(image, texcoordM).r / maxValue);

    if(dist <= 0.0)
    {
        gl_FragColor.rgb = vec3(0.0,0.0,0.0);
    }
    else if(dist <= max3)
    {
        gl_FragColor.gb = vec2(0.0,0.0);
        gl_FragColor.r = dist / max3;
    }
    else if(dist <= 2.0*max3)
    {
        gl_FragColor.r = 1.0;
        gl_FragColor.g = (dist - max3) / max3;
        gl_FragColor.b = 0.0;
    }
    else
    {
        gl_FragColor.b = (dist - 2.0*max3) / max3;
        gl_FragColor.g = 1.0;
        gl_FragColor.r = 1.0;
    }
    
    gl_FragColor.a = dist * 2.0;
}
