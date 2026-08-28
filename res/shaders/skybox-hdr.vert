#version 460 core

layout(location = 0) in vec3 a_pos;

out vec3 v_localPos;

uniform mat4 u_camProj;
uniform mat4 u_camView;

void main() {
  v_localPos = a_pos;

  mat4 rotView = mat4(mat3(u_camView));
  vec4 clipPos = u_camProj * rotView * vec4(a_pos, 1.f);

  gl_Position = clipPos.xyww;
}

