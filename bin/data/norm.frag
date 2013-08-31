uniform sampler2DRect image;
uniform float maxValue;
varying vec2 texcoordM;

void main() {
    vec4 t = texture2DRect(image, texcoordM);
    
    gl_FragColor = t/maxValue;
    
}
