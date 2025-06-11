#ifndef CONE
#define CONE

#include "../utils/list.hpp"
#include "../utils/point.hpp"
#include "../utils/primitive.hpp"
#include "plane.hpp"

Primitive generateCone(float radius, float height, int slices, int stacks);

#endif