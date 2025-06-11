#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include "../tinyXML/tinyxml2.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

struct Point {
    float x;
    float y;
    float z;
};