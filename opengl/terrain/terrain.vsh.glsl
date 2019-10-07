#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV0;
layout(location = 3) in vec3 aTangent;

out VertexData
{
  vec3 position;
  vec3 normal;
  vec3 tangent;
  float altitude;
  float eyeDistance;
} vOutput;

uniform mat4 uModelViewProjection;
uniform mat4 uModel;
uniform vec2 uMinMaxAltitudes;
uniform vec3 uCameraPos;

void main()
{
  gl_Position = uModelViewProjection * vec4(aPosition, 1.0);

  vOutput.position = aPosition.xyz;
  vOutput.normal = (uModel * vec4(normalize(aNormal), 0)).xyz;
  vOutput.tangent = (uModel * vec4(normalize(aTangent), 0)).xyz;
  vOutput.altitude = clamp((aPosition.y - uMinMaxAltitudes.x) / (uMinMaxAltitudes.y - uMinMaxAltitudes.x), 0.0, 1.0);
  vec3 eyeVec = (uModel * vec4(aPosition, 1.0)).xyz - uCameraPos;
  vOutput.eyeDistance = dot(eyeVec, eyeVec);
}
