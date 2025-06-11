#include "cone.hpp"

#include <iostream>
#include <cmath>

#define _USE_MATH_DEFINES

using std::cerr, std::endl;

Primitive generateCone(float bottomRadius, float height, int slices, int stacks) {
    Primitive cone = newEmptyPrimitive();
    if (!cone) return cone;

    float alpha = (2 * M_PI) / slices;  // Ângulo entre slices
    float deltaRadius = bottomRadius / stacks;  // Variação do raio por stack
    float deltaHeight = height / stacks;  // Variação da altura por stack

    // Base do cone (Círculo)
    for (int i = 0; i < slices; i++) {
        float x1 = bottomRadius * sin(alpha * i);
        float z1 = bottomRadius * cos(alpha * i);
        float x2 = bottomRadius * sin(alpha * (i + 1));
        float z2 = bottomRadius * cos(alpha * (i + 1));

        addValueList(getPontos(cone), newPoint(0.0f, 0.0f, 0.0f)); // Centro da base
        addValueList(getPontos(cone), newPoint(x2, 0.0f, z2));
        addValueList(getPontos(cone), newPoint(x1, 0.0f, z1));
    }

    // Laterais do cone
    for (int j = 0; j < stacks; j++) {
        float r1 = bottomRadius - j * deltaRadius;
        float r2 = bottomRadius - (j + 1) * deltaRadius;
        float y1 = j * deltaHeight;
        float y2 = (j + 1) * deltaHeight;

        for (int i = 0; i < slices; i++) {
            float x1 = r1 * sin(alpha * i);
            float z1 = r1 * cos(alpha * i);
            float x2 = r1 * sin(alpha * (i + 1));
            float z2 = r1 * cos(alpha * (i + 1));

            float x3 = r2 * sin(alpha * i);
            float z3 = r2 * cos(alpha * i);
            float x4 = r2 * sin(alpha * (i + 1));
            float z4 = r2 * cos(alpha * (i + 1));

            // Primeiro triângulo
            addValueList(getPontos(cone), newPoint(x1, y1, z1));
            addValueList(getPontos(cone), newPoint(x2, y1, z2));
            addValueList(getPontos(cone), newPoint(x3, y2, z3));

            // Segundo triângulo
            addValueList(getPontos(cone), newPoint(x2, y1, z2));
            addValueList(getPontos(cone), newPoint(x4, y2, z4));
            addValueList(getPontos(cone), newPoint(x3, y2, z3));
        }
    }

    // Ponta do cone (único ponto no topo)
    float tipX = 0.0f, tipY = height, tipZ = 0.0f;
    for (int i = 0; i < slices; i++) {
        float x1 = (bottomRadius - stacks * deltaRadius) * sin(alpha * i);
        float z1 = (bottomRadius - stacks * deltaRadius) * cos(alpha * i);
        float x2 = (bottomRadius - stacks * deltaRadius) * sin(alpha * (i + 1));
        float z2 = (bottomRadius - stacks * deltaRadius) * cos(alpha * (i + 1));

        addValueList(getPontos(cone), newPoint(x1, height - deltaHeight, z1));
        addValueList(getPontos(cone), newPoint(x2, height - deltaHeight, z2));
        addValueList(getPontos(cone), newPoint(tipX, tipY, tipZ)); // Ponta do cone
    }

    return cone;
}
