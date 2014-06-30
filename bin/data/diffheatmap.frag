uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;
uniform float maxValue1;
uniform float maxValue2;
//uniform float Opacity;

void main (void)
{
    
    float dist = (texture2DRect(img1, texcoordM).a - texture2DRect(img2, texcoordM).a) / (min(maxValue1,maxValue2) / 2.0);
    float max2 = 1.0 / 2.0;
    
    if(dist < (-1.0 * max2))
    {
        gl_FragColor.r = 0.0;
        gl_FragColor.g = ((-1.0 * dist) - max2) / max2;
        gl_FragColor.b = 1.0;
    }
    else if(dist < 0.0)
    {
    	gl_FragColor.r = 0.0;
        gl_FragColor.b = (-1.0 * dist) / max2;
        gl_FragColor.g = 0.0;
    }
    else if(dist == 0.0)
    {
        gl_FragColor.rgb = vec3(0.0,0.0,0.0);
    }
    else if(dist < max2)
    {
        gl_FragColor.gb = vec2(0.0,0.0);
        gl_FragColor.r = dist / max2;
    }
    else //if(dist <= 2.0*max3)
    {
        gl_FragColor.r = 1.0;
        gl_FragColor.g = (dist - max2) / max2;
        gl_FragColor.b = 0.0;
    }
//    else
//    {
//        gl_FragColor.b = (dist - 2.0*max3) / max3;
//        gl_FragColor.g = 1.0;
//        gl_FragColor.r = 1.0;
//    }
    
    gl_FragColor.a = clamp(abs(dist), 0.0, 0.95);
}