#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex;

out vec3 v_worldPos;
out vec3 v_normal;
out vec2 v_uv;

uniform mat4 u_model;
uniform mat4 u_camPV;
uniform float u_radius;

vec3 cubeToSphere(vec2 pos, int faceIdx) {
  float u = pos.x;
  float v = pos.y;
  vec3 p;

  switch (faceIdx) {
    case 0: p = vec3( 1,  v,  u); break; // Right
    case 1: p = vec3(-1,  v, -u); break; // Left
    case 2: p = vec3( u,  1,  v); break; // Top
    case 3: p = vec3(-u, -1,  v); break; // Bottom
    case 4: p = vec3(-u,  v,  1); break; // Front
    case 5: p = vec3( u,  v, -1); break; // Back
  }

  return p;
}

void main() {
  vec3 sphereDir = normalize(cubeToSphere(a_pos.xz, gl_InstanceID));
  vec4 worldPos = u_model * vec4(sphereDir * u_radius, 1.f);

  v_worldPos = worldPos.xyz;
  v_normal = a_pos;
  v_uv = a_tex;
	gl_Position = u_camPV * worldPos;
}

