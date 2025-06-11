#include <stdlib.h>
#include "box.hpp"

struct point
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};
#include <math.h>

Point newPointFull(float x, float y, float z, float nx, float ny, float nz, float u, float v)
{
    Point p = (Point)malloc(sizeof(struct point));
    p->x = x;
    p->y = y;
    p->z = z;
    p->nx = nx;
    p->ny = ny;
    p->nz = nz;
    p->u = u;
    p->v = v;
    return p;
}

void addFace(Primitive box, float x1, float y1, float z1,
             float x2, float y2, float z2,
             float x3, float y3, float z3,
             float x4, float y4, float z4,
             float nx, float ny, float nz,
             float u0, float v0, float u1, float v1)
{
    // Triangle 1
    addValueList(getPontos(box), newPointFull(x3, y3, z3, nx, ny, nz, u0, v1));
    addValueList(getPontos(box), newPointFull(x2, y2, z2, nx, ny, nz, u1, v0));
    addValueList(getPontos(box), newPointFull(x1, y1, z1, nx, ny, nz, u0, v0));

    // Triangle 2
    addValueList(getPontos(box), newPointFull(x3, y3, z3, nx, ny, nz, u0, v1));
    addValueList(getPontos(box), newPointFull(x4, y4, z4, nx, ny, nz, u1, v1));
    addValueList(getPontos(box), newPointFull(x2, y2, z2, nx, ny, nz, u1, v0));
}

Primitive generateBox(float size, int divisions)
{
    Primitive box = newEmptyPrimitive();
    float step = size / divisions;

    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {
            float x1 = -size / 2 + i * step;
            float z1 = -size / 2 + j * step;
            float x2 = x1 + step;
            float z2 = z1;
            float x3 = x1;
            float z3 = z1 + step;
            float x4 = x2;
            float z4 = z3;
            float y = size / 2;
            float u0 = i / (float)divisions;
            float u1 = (i + 1) / (float)divisions;
            float v0 = j / (float)divisions;
            float v1 = (j + 1) / (float)divisions;

            // Top (+Y)
            addFace(box, x1, y, z1, x2, y, z2, x3, y, z3, x4, y, z4, 0, 1, 0, u0, v0, u1, v1);
            // Bottom (-Y)
            addFace(box, x3, -y, z3, x1, -y, z1, x4, -y, z4, x2, -y, z2, 0, -1, 0, u0, v0, u1, v1);
            // Front (+Z)
            addFace(box, x1, z1, y, x2, z2, y, x3, z3, y, x4, z4, y, 0, 0, 1, u0, v0, u1, v1);
            // Back (-Z)
            addFace(box, x3, -z3, y, x1, -z1, y, x4, -z4, y, x2, -z2, y, 0, 0, -1, u0, v0, u1, v1);
            // Right (+X)
            addFace(box, y, x1, z1, y, x2, z2, y, x3, z3, y, x4, z4, 1, 0, 0, u0, v0, u1, v1);
            // Left (-X)
            addFace(box, -y, x3, z3, -y, x1, z1, -y, x4, z4, -y, x2, z2, -1, 0, 0, u0, v0, u1, v1);
        }
    }

    return box;
}
