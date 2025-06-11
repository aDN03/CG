#ifndef BEZIER
#define BEZIER

#include "../utils/list.hpp"
#include "../utils/point.hpp"
#include "../utils/primitive.hpp"
#include "../utils/matrix.hpp"

std::vector<std::vector<std::vector<float> > > readPatchesFile(const char* filePath);

Primitive generateSurface(const char* filePath, int tessellation);


#endif