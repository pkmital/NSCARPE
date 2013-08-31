varying vec2 texcoordM;
uniform sampler2DRect image; 
void main (void) 
{ 
	gl_Position = ftransform();
    
    // transform texcoord
	vec2 texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
	
	// get sample positions
    texcoordM = texcoord;
} 