#include "cone.hpp"
#include <math.h>
#include <stdlib.h>
#include <algorithm>

// Estrutura atualizada para incluir normais e UVs
struct point
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

typedef struct point *Point;

// Função para criar um novo ponto com normais e coordenadas de textura
Point newPointWithNormalUV(float x, float y, float z, float nx, float ny, float nz, float u, float v)
{
    Point p = (Point)malloc(sizeof(struct point));
    if (p)
    {
        p->x = x;
        p->y = y;
        p->z = z;
        p->nx = nx;
        p->ny = ny;
        p->nz = nz;
        p->u = u;
        p->v = v;
    }
    return p;
}

Primitive generateSphere(float radius, int slices, int stacks)
{
    Primitive sphere = newEmptyPrimitive();

    for (int i = 0; i < slices; i++)
    {
        float alpha1 = ((2.0f * M_PI) / slices) * i;
        float alpha2 = ((2.0f * M_PI) / slices) * (i + 1);

        for (int j = 0; j < stacks; j++)
        {
            float beta1 = (M_PI / stacks) * j;
            float beta2 = (M_PI / stacks) * (j + 1);

            float x1 = radius * sin(beta1) * cos(alpha1);
            float y1 = radius * cos(beta1);
            float z1 = radius * sin(beta1) * sin(alpha1);

            float x2 = radius * sin(beta2) * cos(alpha1);
            float y2 = radius * cos(beta2);
            float z2 = radius * sin(beta2) * sin(alpha1);

            float x3 = radius * sin(beta2) * cos(alpha2);
            float y3 = radius * cos(beta2);
            float z3 = radius * sin(beta2) * sin(alpha2);

            float x4 = radius * sin(beta1) * cos(alpha2);
            float y4 = radius * cos(beta1);
            float z4 = radius * sin(beta1) * sin(alpha2);

            float u1 = static_cast<float>(i) / slices;
            float u2 = static_cast<float>(i + 1) / slices;
            float v1 = static_cast<float>(j) / stacks;
            float v2 = static_cast<float>(j + 1) / stacks;

            addValueList(getPontos(sphere), newPointWithNormalUV(x2, y2, z2, x2 / radius, y2 / radius, z2 / radius, u1, v2));
            addValueList(getPontos(sphere), newPointWithNormalUV(x1, y1, z1, x1 / radius, y1 / radius, z1 / radius, u1, v1));
            addValueList(getPontos(sphere), newPointWithNormalUV(x3, y3, z3, x3 / radius, y3 / radius, z3 / radius, u2, v2));

            addValueList(getPontos(sphere), newPointWithNormalUV(x4, y4, z4, x4 / radius, y4 / radius, z4 / radius, u2, v1));
            addValueList(getPontos(sphere), newPointWithNormalUV(x3, y3, z3, x3 / radius, y3 / radius, z3 / radius, u2, v2));
            addValueList(getPontos(sphere), newPointWithNormalUV(x1, y1, z1, x1 / radius, y1 / radius, z1 / radius, u1, v1));
        }
    }
    return sphere;
}
