uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;
//uniform float Opacity;
const float PI = 3.14159265;
const float c1 = 0.0;
const float colorMax = 1.0;

void main (void)
{
    float dist = (texture2DRect(img1, texcoordM).a - texture2DRect(img2, texcoordM).a);
    float max = 1.0;
    float max3 = 1.0 / 3.0;
    
    
    if(dist <= (-2.0 * max3))
    {
        gl_FragColor.r = 0.0;
        gl_FragColor.g = dist / max;
        gl_FragColor.b = 1.0 - gl_FragColor.r;
    }
    else if(dist < 0.0)
    {
    	gl_FragColor.r = 0.0;
        gl_FragColor.g = (dist / max) * (dist / (2.0 * max3));
        gl_FragColor.b = (1.0 - gl_FragColor.r) * (dist / (2.0 * max3));
    }
    else if(dist == 0.0)
    {
        gl_FragColor = vec4(0.0);
    }
    else if(dist <= 2.0 * max3)
    {
        gl_FragColor.gb = vec2(0.0,0.0);
        gl_FragColor.r = dist / max3;
    }
    else //if(dist <= 3.0*max3)
    {
        gl_FragColor.r = 1.0;
        gl_FragColor.g = (dist - max3) / max3;
        gl_FragColor.b = 0.0;
    }
//    else
//    {
//        gl_FragColor.b = (dist - 2.0*max3) / max3;
//        gl_FragColor.g = 1.0;
//        gl_FragColor.r = 1.0;
//    }
    
    gl_FragColor.a = abs(dist);
}