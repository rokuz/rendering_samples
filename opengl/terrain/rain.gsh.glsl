#version 410 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VertexData
{
  int index;
} gInput[];

uniform mat4 uViewProjection;
uniform int uRainDropPerSideCount;
uniform float uTime;
uniform vec4 uBounds;
uniform vec3 uCameraPos;

out float vLength;
out float vEyeDistance;

void main()
{
  int i = gInput[0].index / (uRainDropPerSideCount - 1);
  int j = gInput[0].index % (uRainDropPerSideCount - 1);
  float t = uTime + float(gInput[0].index);
  float x = mix(uBounds.x, uBounds.y, float(i) / uRainDropPerSideCount) + sin(t);
  float z = mix(uBounds.z, uBounds.w, float(j) / uRainDropPerSideCount) + cos(t);
  float y0 = 100.0;
  float y1 = cos(t) * 10.0;
  vec3 eyeVec = vec3(x, y1, z) - uCameraPos;
  float eyeDist = dot(eyeVec, eyeVec);

  gl_Position = uViewProjection * vec4(x, y0, z, 1.0);
  vLength = y0;
  vEyeDistance = eyeDist;
  EmitVertex();
  gl_Position = uViewProjection * vec4(x, y1, z, 1.0);
  vLength = y1;
  vEyeDistance = eyeDist;
  EmitVertex();
  EndPrimitive();
}
