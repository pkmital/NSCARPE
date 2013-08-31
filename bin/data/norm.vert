varying vec2 texcoordM;

void main() {
    // get the homogeneous 2d position
    gl_Position = ftransform();
    
    // transform texcoord	
	texcoordM = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
}
