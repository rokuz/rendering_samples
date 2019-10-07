#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 uModel;
uniform float uNormalHardness;

in VertexData
{
  vec3 position;
  vec3 normal;
  vec3 tangent;
  float altitude;
} gInput[];

out vec3 vNormal;
out vec3 vTangent;
out float vAltitude;

void main()
{
  vec3 n1 = normalize(gInput[1].position - gInput[0].position);
  vec3 n2 = normalize(gInput[2].position - gInput[0].position);
  vec3 triangleNormal = (uModel * vec4(cross(n1, n2), 0.0f)).xyz;
  vec3 triangleTangent = (uModel * vec4(cross(triangleNormal, vec3(0.0f, 0.0f, 1.0f)), 0.0f)).xyz;

  for (int i = 0; i < gl_in.length(); i++)
  {
    gl_Position = gl_in[i].gl_Position;
    vNormal = mix(gInput[i].normal, triangleNormal, uNormalHardness);
    vTangent = mix(gInput[i].tangent, triangleTangent, uNormalHardness);
    vAltitude = gInput[i].altitude;
    EmitVertex();
  }
  EndPrimitive();
}
