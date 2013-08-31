varying vec2 texcoordM;
uniform sampler2DRect image; 
void main (void) 
{ 
	vec4 a, b, c, d; 
	a = texture2DRect( image, texcoordM );
	b = texture2DRect( image, texcoordM + vec2(1,0) );
	c = texture2DRect( image, texcoordM + vec2(0,1) ); 
	d = texture2DRect( image, texcoordM + vec2(1,1) );
	gl_FragColor = max( max(a,b), max(c,d) );
} 
