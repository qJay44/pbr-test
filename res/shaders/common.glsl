#define PI 3.14159265358979
#define TAU (2.0 * PI)
#define PHI 1.6180339
#define GOLDEN_ANGLE (TAU * (2.0 - PHI))

#define sq(x) ((x)*(x))
#define clamp01(x) clamp(x, 0.0, 1.0)
#define dot0(x, y) max(dot(x, y), 0.0)
#define length2(v) dot(v, v)
#define normalizeSafe(v) (length2(v) > 1e-6 ? (v) * inversesqrt(length2(v)) : (v) * 0.0)

struct LightPoint {
  vec3 pos;
  vec3 color;
  vec3 multiplier;
  float radius;
};

