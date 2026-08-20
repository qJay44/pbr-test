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

static constexpr f0 all[] = {
  { "Water",               vec3(0.15, 0.15, 0.15) },
  { "Plastic / Glass Low", vec3(0.21, 0.21, 0.21) },
  { "Plastic High",        vec3(0.24, 0.24, 0.24) },
  { "Glass High / Ruby",   vec3(0.31, 0.31, 0.31) },
  { "Diamond",             vec3(0.45, 0.45, 0.45) },
  { "Iron",                vec3(0.77, 0.78, 0.78) },
  { "Copper",              vec3(0.98, 0.82, 0.76) },
  { "Gold",                vec3(1.00, 0.86, 0.57) },
  { "Aluminium",           vec3(0.96, 0.96, 0.97) },
  { "Silver",              vec3(0.98, 0.97, 0.95) }
};

} // namespace f0

