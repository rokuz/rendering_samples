#version 410 core

in vec3 vNormal;
in vec3 vTangent;

out vec4 oColor;

uniform vec4 uDiffuseColor;

const vec3 kLightDir = vec3(1, -1, 1);

void main()
{
  mat3 ts = transpose(mat3(vTangent, cross(vNormal, vTangent), vNormal));
  vec3 lightDir = ts * normalize(kLightDir);
  float ndotl = max(dot(-lightDir, vec3(0, 0, 1)), 0.0);
  
  vec4 diffuse = uDiffuseColor * ndotl;
  vec4 ambient = 0.25 * uDiffuseColor;
  oColor = clamp(diffuse + ambient, 0.0, 1.0);
}
