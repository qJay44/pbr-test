#version 460 core

in vec4 worldPos;
in vec3 color;
in vec2 texCoord;
in vec3 normal;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_debug0Tex;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_camPos;
uniform float u_time;

vec3 directionalLight() {
  vec3 lightDistVec = u_lightPos - worldPos.xyz;
  float lightDist = length(lightDistVec);

  vec3 lightDir = normalize(lightDistVec);
  vec3 viewDir = normalize(u_camPos - worldPos.xyz);
  vec3 reflectDir = reflect(-lightDir, normal);

  float diffuse = max(dot(normal, lightDir), 0.f);
  float ambient = 0.2f;
  float specularLight = 0.5f;
  float specAmount = pow(max(dot(viewDir, reflectDir), 0.f), 8);
  float specular = specAmount * specularLight;

  float lightAmount = (diffuse + specular) + ambient;

  return u_lightColor * lightAmount;
}

void main() {
  vec2 offset = vec2(0.f);
  offset.y = -u_time * 0.1f;

  vec3 col = texture(u_debug0Tex, texCoord + offset).rgb;
  col *= directionalLight();
	FragColor = vec4(col, 1.f);
}

