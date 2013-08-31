uniform sampler2DRect img1;
uniform sampler2DRect img2;
varying vec2 texcoordM;
//uniform float Opacity;

void main (void)
{
    // get the homogeneous 2d position
    gl_Position = ftransform();
    
    // transform texcoord	
	vec2 texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
	
	// get sample positions
    texcoordM = texcoord;
}