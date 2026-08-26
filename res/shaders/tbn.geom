#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 v_worldPos;
out vec3 v_normal;
out vec2 v_uv;
out mat3 v_tbn;

in VERTEX_DATA {
  vec3 normal;
  vec2 uv;
} data_in[];

layout(binding = 3) uniform sampler2D u_texHeight;

uniform mat4 u_model;
uniform mat4 u_camPV;
uniform float u_heightScale;

void main() {
  vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec2 st0 = data_in[1].uv - data_in[0].uv;
  vec2 st1 = data_in[2].uv - data_in[0].uv;

  float invDet = 1.f / (st0.x * st1.y - st1.x * st0.y + 1e-4f);

  vec3 tangent = vec3(invDet * (st1.y * edge0 - st0.y * edge1));
  vec3 bitangent = vec3(invDet * (-st1.x * edge0 + st0.x * edge1));

  mat3 tbn = mat3(
    normalize(vec3(u_model * vec4(tangent, 0.f))),
    normalize(vec3(u_model * vec4(bitangent, 0.f))),
    normalize(vec3(u_model * vec4(cross(edge0, edge1), 0.f)))
  );

  for (int i = 0; i < 3; i++) {
    float h = texture(u_texHeight, data_in[i].uv).r;
    vec3 pos = gl_in[i].gl_Position.xyz;
    pos += data_in[i].normal * h * u_heightScale;

    vec4 worldPos = u_model * vec4(pos, 1.f);

    gl_Position = u_camPV * worldPos;
    v_worldPos = worldPos.xyz;
    v_normal = data_in[i].normal;
    v_uv = data_in[i].uv;
    v_tbn = tbn;
    EmitVertex();
  }

  EndPrimitive();
}

