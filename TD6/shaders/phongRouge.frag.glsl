#version 150 core

out vec4 frag_color;
in vec3 lightDir;
in vec3 eyeVec;
in vec3 out_normal;

void main( void )
{
    vec3 L = normalize(lightDir);
    vec3 N = normalize(out_normal);
    vec3 E = normalize(eyeVec);
    
    float intensity = max(dot(L, N), 0.0);
    

    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, E), 0.0), 32.0); 
    vec4 specular = vec4(1.0, 1.0, 1.0, 1.0) * spec; 

    vec4 diffuse = vec4(0.8, 0.4, 0.4, 1.0) * intensity;
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);

    frag_color = ambient + diffuse + specular;
}