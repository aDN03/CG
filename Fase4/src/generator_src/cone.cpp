#include "cone.hpp"
#include <math.h>
#include <stdlib.h>

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

Primitive generateCone(int radius, int height, int slices, int stacks)
{
    Primitive cone = newEmptyPrimitive();

    if (cone)
    {
        float stack_height = static_cast<float>(height) / stacks;
        float stack_radius = static_cast<float>(radius) / stacks;
        float slice_angle = 2 * M_PI / slices;

        for (int i = 0; i < stacks; i++)
        {
            float z1 = i * stack_height;
            float z2 = (i + 1) * stack_height;
            float r1 = radius - i * stack_radius;
            float r2 = radius - (i + 1) * stack_radius;

            for (int j = 0; j < slices; j++)
            {
                float angle1 = j * slice_angle;
                float angle2 = (j + 1) * slice_angle;

                float x1 = r1 * cos(angle1);
                float y1 = r1 * sin(angle1);

                float x2 = r1 * cos(angle2);
                float y2 = r1 * sin(angle2);

                float x3 = r2 * cos(angle1);
                float y3 = r2 * sin(angle1);

                float x4 = r2 * cos(angle2);
                float y4 = r2 * sin(angle2);

                // Normais aproximadas (calculadas com base no gradiente lateral do cone)
                float nx1 = cos(angle1);
                float ny1 = sin(angle1);
                float nx2 = cos(angle2);
                float ny2 = sin(angle2);

                float u1 = static_cast<float>(j) / slices;
                float u2 = static_cast<float>(j + 1) / slices;
                float v1 = static_cast<float>(i) / stacks;
                float v2 = static_cast<float>(i + 1) / stacks;

                // Primeiro triângulo do quadrado lateral
                addValueList(getPontos(cone), newPointWithNormalUV(x1, z1, y1, nx1, 0, ny1, u1, v1));
                addValueList(getPontos(cone), newPointWithNormalUV(x2, z1, y2, nx2, 0, ny2, u2, v1));
                addValueList(getPontos(cone), newPointWithNormalUV(x3, z2, y3, nx1, 0, ny1, u1, v2));

                // Segundo triângulo do quadrado lateral
                addValueList(getPontos(cone), newPointWithNormalUV(x2, z1, y2, nx2, 0, ny2, u2, v1));
                addValueList(getPontos(cone), newPointWithNormalUV(x4, z2, y4, nx2, 0, ny2, u2, v2));
                addValueList(getPontos(cone), newPointWithNormalUV(x3, z2, y3, nx1, 0, ny1, u1, v2));
            }
        }
    }

    return cone;
}
