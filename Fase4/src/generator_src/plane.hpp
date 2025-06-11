#ifndef PLANE
#define PLANE

#include "../utils/list.hpp"
#include "../utils/point.hpp"
#include "../utils/primitive.hpp"

Primitive generatePlaneXZ(int length, int divisions, float h = 0.0f, int bottom = 0);

#endif