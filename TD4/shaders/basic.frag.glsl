#version 150 core

// couleur émise pour le pixel
out vec4 frag_color;

in vec3 color;

void main(void)
{
  float x = gl_FragCoord.x;
  float stripe = mod(floor(x / 10.0), 3.0); // 3 bandes égales

  vec3 finalColor;

  if (stripe == 0.0)      finalColor = vec3(0.0, 1.0, 0.0); // vert
  else if (stripe == 1.0) finalColor = vec3(1.0, 0.0, 0.0); // rouge
  else                    finalColor = vec3(0.0, 0.0, 0.0); // noir

  frag_color = vec4(finalColor, 1.0);

}
