#version 460 core

in vec3 v_localPos;

out vec4 FragColor;

layout(binding = 0) uniform samplerCube u_texCubamap;

void main() {
  vec3 color = texture(u_texCubamap, v_localPos).rgb;

  color /= color + vec3(1.f);
  color = pow(color, vec3(1.f / 2.2f));

  FragColor = vec4(color, 1.f);
}

