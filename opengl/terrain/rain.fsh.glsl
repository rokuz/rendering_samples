#version 410 core

in float vLength;
in float vEyeDistance;

out vec4 oColor;

void main()
{
  vec3 c = mix(vec3(1.0, 1.0, 1.0), vec3(0.7, 0.7, 0.7), clamp(vEyeDistance / 10000.0, 0.0, 1.0));
  oColor = vec4(c, 0.3 * (0.5 * sin(2.0 * vLength) + 0.5));
}
