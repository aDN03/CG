#include "plane.hpp"
#include <algorithm>

Primitive generatePlaneXZ(int length, int divisions, float h, int bottom) {
    Primitive plano = newEmptyPrimitive();
    h = 0.0f;
    bottom = 0;

    if (plano) {
        float dimension2 = static_cast<float>(length) / 2;
        float div_side = static_cast<float>(length) / divisions;

        for (int linha = 0; linha < divisions; linha++) {
            for (int coluna = 0; coluna < divisions; coluna++) {
                float x1 = -dimension2 + coluna * div_side;
                float z1 = -dimension2 + linha * div_side;

                float x2 = x1;
                float z2 = z1 + div_side;

                float x3 = x1 + div_side;
                float z3 = z1;

                float x4 = x1 + div_side;
                float z4 = z1 + div_side;
                
                if (bottom == 1) {
                    std::swap(z1, z3);
                    std::swap(z2, z4);
                }

                // Primeiro triângulo do quadrado
                addValueList(getPontos(plano), newPoint(x1, h, z1));
                addValueList(getPontos(plano), newPoint(x2, h, z2));
                addValueList(getPontos(plano), newPoint(x3, h, z3));

                // Segundo triângulo do quadrado
                addValueList(getPontos(plano), newPoint(x2, h, z2));
                addValueList(getPontos(plano), newPoint(x4, h, z4));
                addValueList(getPontos(plano), newPoint(x3, h, z3));
            }
        }
    }

    return plano;
}
