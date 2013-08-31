uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;
const float PI = 3.14159265;
const float c1 = 0.0;
const float colorMax = 1.0;
//uniform float Opacity;

void main (void)
{
    
    float dist = clamp(((texture2DRect(img1, texcoordM).a - texture2DRect(img2, texcoordM).a) + 1.0) / 2.0, 0.0, 1.0);
    float max4 = colorMax / 4.0;
    
    if(dist <= max4)
    {
        gl_FragColor.rg = vec2(0.0, 0.0);
        gl_FragColor.b = c1 + ((colorMax - c1) * dist / max4);
    }
    else if(dist < 2.0 * max4)
    {
        gl_FragColor.r = 0.0;
        gl_FragColor.g = (colorMax * (dist - max4) / max4);
        gl_FragColor.b = colorMax;
    }
    
    else if(dist < 3.0 * max4)
    {
        gl_FragColor.r = (colorMax * (dist - 2.0 * max4) / max4);
        gl_FragColor.g = colorMax;
        gl_FragColor.b = colorMax - gl_FragColor.r;
    }
    else if(dist < 4.0 * max4)
    {
        gl_FragColor.r = colorMax;
        gl_FragColor.g = (colorMax - colorMax * (dist - 3.0 * max4) / max4);
        gl_FragColor.b = 0.0;
    }
    else
    {
        gl_FragColor.r = colorMax;
        gl_FragColor.gb = vec2(0.0, 0.0);
    }
    
    gl_FragColor.a = 0.5;
}