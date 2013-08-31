uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;


void main (void)
{
    gl_FragColor = vec4(texture2DRect(img1, texcoordM).a + texture2DRect(img2, texcoordM).a);
}