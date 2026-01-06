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


void main(void)
{
  color = vec4(-in_normal, 0.0);
  // color =in_pos;
  // calcul de la position du point une fois toutes les transformations appliquées
  vec4 vVertex = v*m * vec4(in_pos, 1.0);

  eyeVec = -vVertex.xyz;

  vec4 LightSource_position=vec4(0.0,0.0,10.0,0.0);
  lightDir=vec3(LightSource_position.xyz - vVertex.xyz);

  out_normal = in_normal;
  gl_Position = p*vVertex;
}
