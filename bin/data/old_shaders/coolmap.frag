uniform sampler2DRect image;
uniform float maxValue;
varying vec2 texcoordM;

/*
void main() {
    float max = 1.0;
    float max3 = 1.0 / 3.0;
    float dist = min(1.0, texture2DRect(image, texcoordM).r / maxValue);

    if(dist <= 0.0)
    {
        gl_FragColor.rgb = vec3(0.0,0.0,0.0);
    }
    else if(dist <= (2.0 * max3))
    {
    	gl_FragColor.r = (dist / max) * (dist / (2.0 * max3));
        gl_FragColor.g = (1.0 - gl_FragColor.r) * (dist / (2.0 * max3));
        gl_FragColor.b = 1.0 * (dist / (2.0 * max3)); 
    }
    else
    {
        gl_FragColor.r = dist / max;
        gl_FragColor.g = 1.0 - gl_FragColor.r;
        gl_FragColor.b = 1.0; 
    }
    gl_FragColor.a = dist * 2.0;
}
*/

void main() {
    float colorMax = 1.0;
    float c1 = 0.0;
    float max3 = 1.0 / 3.0;
    float dist = min(1.0, texture2DRect(image, texcoordM).r / maxValue);
    
    
    if(dist <= 0.0)
    {
        gl_FragColor.rgb = vec3(0.0, 0.0, 0.0);
    }
    else if(dist < max3)
    {
        gl_FragColor.rg = vec2(0.0, 0.0);
        gl_FragColor.b = c1 + ((colorMax - c1) * dist / max3);
    }
    else if(dist < 2.0 * max3)
    {
        gl_FragColor.r = 0.0;
        gl_FragColor.g = (colorMax * (dist - max3) / max3);
        gl_FragColor.b = colorMax;
    }
    
    else if(dist < 3.0 * max3) 
    {
        gl_FragColor.r = (colorMax * (dist - 2.0 * max3) / max3);
        gl_FragColor.g = colorMax;
        gl_FragColor.b = colorMax - gl_FragColor.r;
    }
    
    gl_FragColor.a = dist * 2.0;
}
