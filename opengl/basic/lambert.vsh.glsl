#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV0;
layout(location = 3) in vec3 aTangent;

out vec3 vNormal;
out vec3 vTangent;

uniform mat4 uModelViewProjection;
uniform mat4 uModel;

void main()
{
  gl_Position = uModelViewProjection * vec4(aPosition, 1.0);

  vNormal = (uModel * vec4(normalize(aNormal), 0)).xyz;
  vTangent = (uModel * vec4(normalize(aTangent), 0)).xyz;
}
