#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV0;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in uvec4 aBoneIndices;
layout(location = 5) in vec4 aBoneWeights;

const int kMaxBonesNumber = 64;

uniform mat4 uModelViewProjection;
uniform mat4 uModel;
uniform mat4 uBoneTransforms[kMaxBonesNumber];

out vec2 vUV0;
out vec3 vNormal;
out vec3 vTangent;

void main()
{
  mat4 boneTransform = (uBoneTransforms[aBoneIndices[0]] * aBoneWeights[0])
                     + (uBoneTransforms[aBoneIndices[1]] * aBoneWeights[1])
                     + (uBoneTransforms[aBoneIndices[2]] * aBoneWeights[2])
                     + (uBoneTransforms[aBoneIndices[3]] * aBoneWeights[3]);

  vec4 pos = boneTransform * vec4(aPosition, 1.0);
  mat4 modelBone = uModel * boneTransform;
  gl_Position = uModelViewProjection * pos;
  vUV0 = aUV0;
  vNormal = normalize((modelBone * vec4(aNormal, 0.0)).xyz);
  vTangent = normalize((modelBone * vec4(aTangent, 0.0)).xyz);
}
