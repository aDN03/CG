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

Primitive generatePlaneXZ(int length, int divisions, float h, int bottom)
{
    Primitive plano = newEmptyPrimitive();
    h = 0.0f;
    bottom = 0;

    if (plano)
    {
        float dimension2 = static_cast<float>(length) / 2;
        float div_side = static_cast<float>(length) / divisions;

        for (int linha = 0; linha < divisions; linha++)
        {
            for (int coluna = 0; coluna < divisions; coluna++)
            {
                float x1 = -dimension2 + coluna * div_side;
                float z1 = -dimension2 + linha * div_side;

                float x2 = x1;
                float z2 = z1 + div_side;

                float x3 = x1 + div_side;
                float z3 = z1;

                float x4 = x1 + div_side;
                float z4 = z1 + div_side;

                if (bottom == 1)
                {
                    std::swap(z1, z3);
                    std::swap(z2, z4);
                }

                float u1 = static_cast<float>(coluna) / divisions;
                float u2 = static_cast<float>(coluna + 1) / divisions;
                float v1 = static_cast<float>(linha) / divisions;
                float v2 = static_cast<float>(linha + 1) / divisions;

                float nx = 0.0f, ny = 1.0f, nz = 0.0f;

                // Primeiro triângulo do quadrado
                addValueList(getPontos(plano), newPointWithNormalUV(x1, h, z1, nx, ny, nz, u1, v1));
                addValueList(getPontos(plano), newPointWithNormalUV(x2, h, z2, nx, ny, nz, u1, v2));
                addValueList(getPontos(plano), newPointWithNormalUV(x3, h, z3, nx, ny, nz, u2, v1));

                // Segundo triângulo do quadrado
                addValueList(getPontos(plano), newPointWithNormalUV(x2, h, z2, nx, ny, nz, u1, v2));
                addValueList(getPontos(plano), newPointWithNormalUV(x4, h, z4, nx, ny, nz, u2, v2));
                addValueList(getPontos(plano), newPointWithNormalUV(x3, h, z3, nx, ny, nz, u2, v1));
            }
        }
    }

    return plano;
}
