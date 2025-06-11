#include "../utils/primitive.hpp"
#include "../utils/point.hpp"
#include "../utils/list.hpp"
#include "plane.hpp"
#include "box.hpp"
#include "cone.hpp"
#include "sphere.hpp"
#include "ring.hpp"
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#define _USE_MATH_DEFINES
#include <math.h>


int main(int argc, char *argv[]) {
    if (argc >= 5) {
        Primitive primitive;
        const char *file_path;

        if (strcmp(argv[1], "plane") == 0) {
            int length = atoi(argv[2]), divisions = atoi(argv[3]);
            file_path = argv[4];

            // Generate the plane
            primitive = generatePlaneXZ(length, divisions, 0.0, 0);
        }
        else if (strcmp(argv[1], "box") == 0) {
            float length = atoi(argv[2]), divisions = atoi(argv[3]);
            file_path = argv[4];

            // Generate the box
            primitive = generateBox(length, divisions);
        }
        else if (strcmp(argv[1], "sphere") == 0) {
            int radius = atoi(argv[2]), slices = atoi(argv[3]), stacks = atoi(argv[4]);
            file_path = argv[5];

            // Generate the sphere
            primitive = generateSphere(radius, slices, stacks);
        }
        else if (strcmp(argv[1], "cone") == 0) {
           int radius = atoi(argv[2]), height = atoi(argv[3]), slices = atoi(argv[4]), stacks = atoi(argv[5]);
            file_path = argv[6];

            // Generate the cone
            primitive = generateCone(radius, height, slices, stacks);
        } 
        else if(strcmp(argv[1], "ring") == 0){
            float ri = atof(argv[2]), re = atof(argv[3]), slices = atoi(argv[4]);
            file_path = argv[5];

            primitive = generateRing(ri,re,slices);  
        }
        else {
            // Invalid shape
            std::cerr << "Invalid shape." << std::endl;
            return 1;
        }

        primitiveToFile(primitive, file_path);
        //freePri(primitive);
    }
    else {
        // Invalid number of arguments
        std::cerr << "Invalid number of arguments." << std::endl;
        return 1;
    }

    return 0;
}
