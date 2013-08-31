uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;
//uniform float Opacity;

void main (void)
{
    float im1  = texture2DRect(img1, texcoordM).r;
    float im2 = texture2DRect(img2, texcoordM).r;
    float max2 = 1.0;
    float diff = im1 - im2;
    
    if(diff == 0.0)
    {
        gl_FragColor.rgb = vec3(0.0, 0.0, 0.0);
    }
    else if(diff > 0.0)
    {
        gl_FragColor.r = min(1.0, diff / max2 * (0.59 / 0.3));
        gl_FragColor.b = 0.0;
        gl_FragColor.g = 0.0;
    }
    else
    {
        gl_FragColor.r = 0.0;
        gl_FragColor.b = 0.0;
        gl_FragColor.g = min(1.0, (-1.0 * diff / max2));
    }

    gl_FragColor.a = 1.0;
}