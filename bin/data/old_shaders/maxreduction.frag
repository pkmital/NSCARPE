varying vec2 texcoord; 
uniform sampler2DRect image; 
void main (void) 
{ 
	vec4 a, b, c, d; 
	a = texture2DRect( image, texcoord ); 
	b = texture2DRect( image, texcoord + vec2(0,1) ); 
	c = texture2DRect( image, texcoord + vec2(1,0) ); 
	d = texture2DRect( image, texcoord + vec2(1,1) ); 
	gl_FragColor = max( max(a,b), max(c,d) ); 
} 
