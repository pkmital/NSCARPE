uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;


void main (void)
{
    gl_FragColor = abs(texture2DRect(img1, texcoordM) - texture2DRect(img2, texcoordM));
}