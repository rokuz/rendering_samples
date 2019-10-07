#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV0;
layout(location = 3) in vec3 aTangent;

out VertexData
{
  vec4 position;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
} vOutput;

void main()
{
  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
  vOutput.position = vec4(aPosition, 1.0);
  vOutput.normal = aNormal;
  vOutput.tangent = aTangent;
  vOutput.binormal = cross(aNormal, aTangent);
}
