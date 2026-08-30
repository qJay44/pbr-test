#version 460 core

out vec2 v_uv;

const vec2 vertices[] = vec2[](
  vec2(-1, -1),
  vec2(-1,  1),
  vec2( 1,  1),
  vec2( 1,  1),
  vec2( 1, -1),
  vec2(-1, -1)
);

void main() {
  vec2 pos = vertices[gl_VertexID];
  v_uv = pos * 0.5f + 0.5f;
	gl_Position = vec4(pos, 1.f, 1.f);
}

