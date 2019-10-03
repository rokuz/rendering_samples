#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV0;
layout(location = 3) in vec3 aTangent;

uniform mat4 uModelViewProjection;

void main()
{
  gl_Position = uModelViewProjection * vec4(aPosition, 1.0);
}
