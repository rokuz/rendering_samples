#version 410 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 uModelViewProjection;

in VertexData
{
  vec4 position;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
} gInput[];

out vec4 color;

void main()
{
  for (int i = 0; i < gl_in.length(); i++)
  {
    gl_Position = uModelViewProjection * gInput[i].position;
    color = vec4(0.0, 0.0, 1.0, 1.0);
    EmitVertex();
    gl_Position = uModelViewProjection * (gInput[i].position + vec4(0.5 * gInput[i].normal, 0));
    color = vec4(0.0, 0.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();

    gl_Position = uModelViewProjection * gInput[i].position;
    color = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();
    gl_Position = uModelViewProjection * (gInput[i].position + vec4(0.5 * gInput[i].tangent, 0));
    color = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();

    gl_Position = uModelViewProjection * gInput[i].position;
    color = vec4(1.0, 1.0, 0.0, 1.0);
    EmitVertex();
    gl_Position = uModelViewProjection * (gInput[i].position + vec4(0.5 * gInput[i].binormal, 0));
    color = vec4(1.0, 1.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
  }
}
