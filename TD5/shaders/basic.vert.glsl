#version 150 core
// version du langage GLSL utilisée, ici 4.5

// mvp est la variable contenant la matrice proj*view*model
// uniform indique que c'est la même matrice pour tous les points
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;


// in indique que la variable est fournie en entrée pour chaque point
// chaque point possède une position 3D
in vec3 in_pos;
in vec3 in_normal;

out vec3 lightDir;
out vec3 eyeVec;
out vec3 out_normal;

out vec4 color;

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main(void)
{
  color = vec4(-in_normal, 0.0);
  // color =in_pos;
  // calcul de la position du point une fois toutes les transformations appliquées
  vec4 vVertex = v*m * vec4(in_pos, 1.0);

  eyeVec = -vVertex.xyz;

  vec4 LightSource_position=vec4(0.0,0.0,10.0,0.0);
  lightDir=vec3(LightSource_position.xyz - vVertex.xyz);

  // On crée un petit décalage basé sur les positions X, Y, Z
  vec3 noise = vec3(
      random(in_pos.xy), 
      random(in_pos.xz), 
      random(in_pos.yz)
  );

  // On ajoute 0.2 * ce bruit à la normale d'origine
  out_normal = vec3(v * m * vec4(in_normal + 0.2 * noise, 0.0));

  // out_normal = vec3(v * m * vec4(in_normal, 0.0));
  // out_normal =  in_normal;
  
  gl_Position = p*vVertex;
}
