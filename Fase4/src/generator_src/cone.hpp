#ifndef CONE
#define CONE

#include "../utils/list.hpp"
#include "../utils/point.hpp"
#include "../utils/primitive.hpp"
#include "plane.hpp"

Primitive generateCone(int radius, int height, int slices, int stacks);

#endif