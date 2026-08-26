#pragma once

namespace f0 {

struct f0 {
  const char* name;
  vec3 color;
};

enum index {
  IDX_WATER,
  IDX_PLASTIC_GLASS_LOW,
  IDX_PLASTIC_HIGH,
  IDX_GLASS_RUBY,
  IDX_DIAMOND,
  IDX_IRON,
  IDX_COPPER,
  IDX_GOLD,
  IDX_ALUMINIUM,
  IDX_SILVER,
  IDX_COUNT,
};

// Linear space
static constexpr f0 all[] = {
  { "Water",               vec3(0.02, 0.02, 0.02) },
  { "Plastic / Glass Low", vec3(0.03, 0.03, 0.03) },
  { "Plastic High",        vec3(0.05, 0.05, 0.05) },
  { "Glass High / Ruby",   vec3(0.08, 0.08, 0.08) },
  { "Diamond",             vec3(0.17, 0.17, 0.17) },
  { "Iron",                vec3(0.56, 0.57, 0.58) },
  { "Copper",              vec3(0.95, 0.64, 0.54) },
  { "Gold",                vec3(1.00, 0.71, 0.29) },
  { "Aluminium",           vec3(0.91, 0.92, 0.92) },
  { "Silver",              vec3(0.95, 0.93, 0.88) }
};

} // namespace f0

