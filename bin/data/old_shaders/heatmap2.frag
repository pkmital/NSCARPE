uniform sampler2DRect image;
uniform float maxValue;
varying vec2 texcoordM;

vec3 RGB2Lab(in vec3 rgb){
    float R = rgb.x;
    float G = rgb.y;
    float B = rgb.z;
    // threshold
    float T = 0.008856;
    
    float X = R * 0.412453 + G * 0.357580 + B * 0.180423;
    float Y = R * 0.212671 + G * 0.715160 + B * 0.072169;
    float Z = R * 0.019334 + G * 0.119193 + B * 0.950227;
    
    // Normalize for D65 white point
    X = X / 0.950456;
    Y = Y;
    Z = Z / 1.088754;
    
    bool XT, YT, ZT;
    XT = false; YT=false; ZT=false;
    if(X > T) XT = true;
    if(Y > T) YT = true;
    if(Z > T) ZT = true;
    
    float Y3 = pow(Y,1.0/3.0);
    float fX, fY, fZ;
    if(XT){ fX = pow(X, 1.0/3.0);} else{ fX = 7.787 * X + 16.0/116.0; }
    if(YT){ fY = Y3; } else{ fY = 7.787 * Y + 16.0/116.0 ; }
    if(ZT){ fZ = pow(Z,1.0/3.0); } else{ fZ = 7.787 * Z + 16.0/116.0; }
    
    float L; if(YT){ L = (116.0 * Y3) - 16.0; }else { L = 903.3 * Y; }
    float a = 500.0 * ( fX - fY );
    float b = 200.0 * ( fY - fZ );
    
    return vec3(L,a,b);
}

vec3 Lab2RGB(in vec3 lab){
    //Thresholds
    float T1 = 0.008856;
    float T2 = 0.206893;
    
    float X,Y,Z;
    
    //Compute Y
    bool XT, YT, ZT;
    XT = false; YT = false; ZT = false;
    
    float fY = pow(((lab.x + 16.0) / 116.0),3.0);
    if(fY > T1){ YT = true; }
    if(YT){ fY = fY; } else{ fY = (lab.x / 903.3); }
    Y = fY;
    
    //Alter fY slightly for further calculations
    if(YT){ fY = pow(fY,1.0/3.0); } else{ fY = (7.787 * fY + 16.0/116.0); }
    
    //Compute X
    float fX = ( lab.y / 500.0 ) + fY;
    if(fX > T2){ XT = true; }
    if(XT){ X = pow(fX,3.0); } else{X = ((fX - (16.0/116.0)) / 7.787); }
    
    //Compute Z
    float fZ = fY - ( lab.z / 200.0 );
    if(fZ > T2){ ZT = true; }
    if(ZT){ Z = pow(fZ,3.0); } else{ Z = ((fZ - (16.0/116.0)) / 7.787); }
    
    //Normalize for D65 white point
    X = X * 0.950456;
    Z = Z * 1.088754;
    
    //XYZ to RGB part
    float R =  3.240479 * X + -1.537150 * Y + -0.498535 * Z;
    float G = -0.969256 * X +  1.875991 * Y +  0.041556 * Z;
    float B =  0.055648 * X + -0.204043 * Y +  1.057311 * Z;
    
    return vec3(R,G,B);
}

void main() {
    float dist = min(1.0, texture2DRect(image, texcoordM).r / maxValue);
    float astar = dist * 100.0 - 50.0;
    float bstar = 0.0;
    float lum = 50.0;
    
    gl_FragColor.rgb = Lab2RGB(vec3(lum, astar, bstar));
    gl_FragColor.a = dist;
}
