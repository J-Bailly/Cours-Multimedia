#version 150 core

// couleur émise pour le pixel
in vec4 color;

out vec4 frag_color;
in vec3 lightDir;
in vec3 eyeVec;
in vec3 out_normal;

void main( void )
{
    vec3 L = normalize(lightDir);
    vec3 N = normalize(out_normal);

    float intensity = max(dot(L,N),0.0);
    frag_color = vec4(intensity,intensity,intensity, 1.0);
   //frag_color = vec4( mod(ceil(gl_FragCoord.x/30)+ceil(gl_FragCoord.y/10), 2), 0.0, 0.0, 1.0 );
   //frag_color = vec4( pow(cos(gl_FragCoord.x*0.02),2.0), 0.0, 0.0, 1.0 );
}