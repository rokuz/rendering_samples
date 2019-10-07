#version 410 core

in vec3 vNormal;
in vec3 vTangent;
in float vAltitude;
in float vEyeDistance;

out vec4 oColor;

uniform mat4 uColors;
uniform vec4 uWeights;

const vec3 kLightDir = vec3(1.0f, -1.0f, 1.0f);

void main()
{
  mat3 ts = transpose(mat3(vTangent, cross(vNormal, vTangent), vNormal));
  vec3 lightDir = ts * normalize(kLightDir);
  float ndotl = max(dot(-lightDir, vec3(0.0f, 0.0f, 1.0f)), 0.0f);

  vec4 diffColor;
  if (vAltitude < uWeights[1])
  {
    float k = clamp((vAltitude - uWeights[0]) / (uWeights[1] - uWeights[0]), 0.0, 1.0);
    diffColor = mix(uColors[0], uColors[1], k);
  }
  else if (vAltitude > uWeights[2])
  {
    float k = clamp((vAltitude - uWeights[2]) / (uWeights[3] - uWeights[2]), 0.0, 1.0);
    diffColor = mix(uColors[2], uColors[3], k);
  }
  else
  {
    float k = clamp((vAltitude - uWeights[1]) / (uWeights[2] - uWeights[1]), 0.0, 1.0);
    diffColor = mix(uColors[1], uColors[2], k);
  }

  vec4 diffuse = diffColor * ndotl;
  vec4 ambient = 0.25f * diffColor;

  float fogDensity = 0.02 * (1.0 - vAltitude);
  float fogCoef = 1.0 - clamp(1.0 / exp(vEyeDistance * fogDensity * fogDensity), 0.0, 1.0);

  oColor = mix(clamp(diffuse + ambient, 0.0f, 1.0f), vec4(1.0, 1.0, 1.0, 1.0), fogCoef);
}
