#include "sphere.hpp"

Primitive generateSphere(float radius, int slices, int stacks) {
    Primitive sphere = newEmptyPrimitive();

    for (int i = 0; i < slices; i++) {
        float alpha1 = ((2.0f * M_PI) / slices) * i;
        float alpha2 = ((2.0f * M_PI) / slices) * (i + 1);

        for (int j = 0; j < stacks; j++) {
            float beta1 = (M_PI / stacks) * j;
            float beta2 = (M_PI / stacks) * (j + 1);

            // 4 pontos da "quadra"
            Point point1 = newPoint(radius * sin(beta1) * cos(alpha1), radius * cos(beta1), radius * sin(beta1) * sin(alpha1));
            Point point2 = newPoint(radius * sin(beta2) * cos(alpha1), radius * cos(beta2), radius * sin(beta2) * sin(alpha1));
            Point point3 = newPoint(radius * sin(beta2) * cos(alpha2), radius * cos(beta2), radius * sin(beta2) * sin(alpha2));
            Point point4 = newPoint(radius * sin(beta1) * cos(alpha2), radius * cos(beta1), radius * sin(beta1) * sin(alpha2));
            // Triângulo 1
            addValueList(getPontos(sphere), point2);
            addValueList(getPontos(sphere), point1);
            addValueList(getPontos(sphere), point3);
            // Triângulo 2
            addValueList(getPontos(sphere), point4);
            addValueList(getPontos(sphere), point3);
            addValueList(getPontos(sphere), point1);

       }
    }
    return sphere;
}
