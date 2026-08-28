#version 460 core

in vec3 v_localPos;

out vec4 FragColor;

layout(binding = 0) uniform samplerCube u_texCubamap;

void main() {
  vec3 color = texture(u_texCubamap, v_localPos).rgb;
  bvec3 b = lessThan(color, vec3(0.f));
  if (b.x || b.y)
    color = vec3(1.f, 0.f, 0.f);

  color /= color + vec3(1.f);
  color = pow(color, vec3(1.f / 2.2f));

  FragColor = vec4(color, 1.f);
}

