#version 410 core

in vec2 vUV0;
in vec3 vNormal;
in vec3 vTangent;

out vec4 oColor;

uniform sampler2D uDiffuseSampler;

const vec3 kLightDir = vec3(1.0f, -1.0f, 1.0f);

void main()
{
  mat3 ts = inverse(mat3(vTangent, cross(vNormal, vTangent), vNormal));
  vec3 lightDir = ts * normalize(kLightDir);
  float ndotl = clamp(dot(-lightDir, vec3(0.0f, 0.0f, 1.0f)), 0.0f, 1.0f);
  vec3 diffColor = texture(uDiffuseSampler, vUV0).rgb;
  vec3 diffuse = diffColor * ndotl;
  vec3 ambient = 0.25f * diffColor;
  oColor = clamp(vec4(diffuse + ambient, 1.0f), 0.0f, 1.0f);
}
