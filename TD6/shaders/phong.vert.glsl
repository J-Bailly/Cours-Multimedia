#version 150 core
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

in vec3 in_pos;
in vec3 in_normal;

out vec3 lightDir;
out vec3 eyeVec;
out vec3 out_normal;

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main(void)
{
  vec4 vVertex = v * m * vec4(in_pos, 1.0);

  eyeVec = -vVertex.xyz;

  vec4 LightSource_position = vec4(0.0, 5.0, 10.0, 1.0); 
  lightDir = vec3(LightSource_position.xyz - vVertex.xyz);

  vec3 noise = vec3(
      random(in_pos.xy), 
      random(in_pos.xz), 
      random(in_pos.yz)
  );
  out_normal = vec3(v * m * vec4(in_normal + 0.05 * noise, 0.0));

  gl_Position = p * vVertex;
}