#include "cone.hpp"

Primitive generateCone(int radius, int height, int slices, int stacks) {
    Primitive cone = newEmptyPrimitive();

    if (cone) {
        float stack_height = static_cast<float>(height) / stacks;
        float stack_radius = static_cast<float>(radius) / stacks;
        float slice_angle = 2 * M_PI / slices;

        for (int i = 0; i < stacks; i++) {
            float z1 = i * stack_height;
            float z2 = (i + 1) * stack_height;
            float r1 = radius - i * stack_radius;
            float r2 = radius - (i + 1) * stack_radius;

            for (int j = 0; j < slices; j++) {
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

                // Primeiro triângulo do quadrado
                addValueList(getPontos(cone), newPoint(x1, z1, y1));
                addValueList(getPontos(cone), newPoint(x2, z1, y2));
                addValueList(getPontos(cone), newPoint(x3, z2, y3));

                // Segundo triângulo do quadrado
                addValueList(getPontos(cone), newPoint(x2, z1, y2));
                addValueList(getPontos(cone), newPoint(x4, z2, y4));
                addValueList(getPontos(cone), newPoint(x3, z2, y3));
            }
        }
    }

    return cone;
}