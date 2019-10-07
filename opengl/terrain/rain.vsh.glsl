#version 410 core

layout(location = 0) in vec3 aPosition;

out VertexData
{
  int index;
} vOutput;

void main()
{
  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
  vOutput.index = gl_InstanceID;
}
