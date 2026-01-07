#version 150 core

out vec4 frag_color;
in vec3 lightDir;
in vec3 eyeVec;
in vec3 out_normal;

vec4 toonify(in float intensity) {
    vec4 color;
    if (intensity > 0.98) 
        color = vec4(0.8, 0.8, 0.8, 1.0);
    else if (intensity > 0.5) 
        color = vec4(0.4, 0.4, 0.8, 1.0);
    else if (intensity > 0.25) 
        color = vec4(0.2, 0.2, 0.4, 1.0);
    else 
        color = vec4(0.1, 0.1, 0.1, 1.0);
    return color;
}

void main( void )
{
    vec3 L = normalize(lightDir);
    vec3 N = normalize(out_normal);
    vec3 E = normalize(eyeVec);
    
    // 1. Composante Diffuse (Lambert)
    float intensity = max(dot(L, N), 0.0);
    vec4 diffuse = vec4(0.4, 0.4, 0.8, 1.0) * intensity; // Lapin bleu

    // 2. Composante Spéculaire (Le reflet brillant)
    vec3 R = reflect(-L, N); // Calcul du rebond de la lumière
    float spec = pow(max(dot(R, E), 0.0), 32.0); // 32 = taille du reflet
    vec4 specular = vec4(1.0, 1.0, 1.0, 1.0) * spec; // Reflet blanc

    // 3. Composante Ambiante (Pour ne pas avoir de noir total)
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);

    frag_color = ambient + diffuse + specular;
}