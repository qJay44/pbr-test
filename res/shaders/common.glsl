#define PI 3.14159265358979f
#define TAU (2.f * PI)
#define PHI 1.6180339f
#define GOLDEN_ANGLE (TAU * (2.f - PHI))

#define sq(x) ((x)*(x))
#define clamp01(x) clamp(x, 0.f, 1.f)
#define dot0(x, y) max(dot(x, y), 0.f)

struct LightPoint {
  vec3 pos;
  vec3 color;
  vec3 multiplier;
  float radius;
};

