varying vec2 texcoord;
uniform sampler2DRect image;

void main()
{
	// perform standard transform on vertex
	gl_Position = ftransform();
    // transform texcoord
	texcoord = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
}