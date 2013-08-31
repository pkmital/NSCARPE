varying vec2 texcoord; 
uniform sampler2DRect image; 
void main (void) 
{ 
	gl_Position = ftransform(); 
	texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0); 
} 