#include "point.hpp"

struct point
{
    float x;
    float y;
    float z;
};

//construtor
Point newPoint(float x, float y, float z){
    Point newP = (Point)malloc(sizeof(Point));
    newP->x = x;
    newP->y = y;
    newP->z = z;
return newP;
}

//gets
float getX(Point ponto){
    if(ponto){
        return ponto->x;
    }
return 0.0f;
}

float getY(Point ponto){
    if(ponto){
        return ponto->y;
    }
return 0.0f;
}

float getZ(Point ponto){
    if(ponto){
        return ponto->z;
    }
return 0.0f;
}

//free
void freeP(Point ponto) {
    free(ponto);
}

//distance
float calculateDistance(Point ponto1, Point ponto2) {
    return sqrt(
        pow((ponto1->x - ponto2->x), 2) +
        pow((ponto1->y - ponto2->y), 2) +
        pow((ponto1->z - ponto2->z), 2)
    );
}


//spherical to sartesian coordinate converter
Point newPontoSph(float a, float b, float radius) {
    if (radius < 0.0f) {
        radius = 0.0f;
    }

    float z = radius * cos(b) * cos(a);
    float x = radius * cos(b) * sin(a);
    float y = radius * sin(b);


return newPoint(x, y, z);
}
