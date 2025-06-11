#include "ring.hpp"
#include <math.h>
#include <stdlib.h>

// A estrutura `Point` e `newPointWithNormalUV` já estão definidas no ficheiro atual.
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

// Gera um anel no plano XZ com normais e UVs
Primitive generateRing(float ri, float re, int slices)
{
    Primitive ring = newEmptyPrimitive();
    float a = 0.0f, delta = (2 * M_PI) / slices;

    if (ring)
    {
        for (int i = 0; i < slices; i++, a += delta)
        {
            float a_next = a + delta;

            // Pontos no raio interno
            float xi1 = ri * cos(a), zi1 = ri * sin(a);
            float xi2 = ri * cos(a_next), zi2 = ri * sin(a_next);

            // Pontos no raio externo
            float xo1 = re * cos(a), zo1 = re * sin(a);
            float xo2 = re * cos(a_next), zo2 = re * sin(a_next);

            // Normais apontam para cima no plano XZ
            float nx = 0.0f, ny = 1.0f, nz = 0.0f;

            // UVs aproximadas para mapear a faixa do anel
            float u1 = 0.5f + 0.5f * cos(a), v1 = 0.5f + 0.5f * sin(a);
            float u2 = 0.5f + 0.5f * cos(a_next), v2 = 0.5f + 0.5f * sin(a_next);

            float ui1 = 0.5f + 0.5f * (ri / re) * cos(a);
            float vi1 = 0.5f + 0.5f * (ri / re) * sin(a);
            float ui2 = 0.5f + 0.5f * (ri / re) * cos(a_next);
            float vi2 = 0.5f + 0.5f * (ri / re) * sin(a_next);

            // Primeiro triângulo
            addValueList(getPontos(ring), newPointWithNormalUV(xi1, 0.0f, zi1, nx, ny, nz, ui1, vi1));
            addValueList(getPontos(ring), newPointWithNormalUV(xo1, 0.0f, zo1, nx, ny, nz, u1, v1));
            addValueList(getPontos(ring), newPointWithNormalUV(xi2, 0.0f, zi2, nx, ny, nz, ui2, vi2));

            // Segundo triângulo
            addValueList(getPontos(ring), newPointWithNormalUV(xi2, 0.0f, zi2, nx, ny, nz, ui2, vi2));
            addValueList(getPontos(ring), newPointWithNormalUV(xo1, 0.0f, zo1, nx, ny, nz, u1, v1));
            addValueList(getPontos(ring), newPointWithNormalUV(xo2, 0.0f, zo2, nx, ny, nz, u2, v2));
        }
    }

    return ring;
}
