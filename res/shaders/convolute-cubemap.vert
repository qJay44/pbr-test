#version 460 core

layout (location = 0) in vec3 a_pos;

out vec3 v_localPos;

uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
  v_localPos = a_pos;
	gl_Position = u_proj * u_view * vec4(a_pos, 1.f);
}

