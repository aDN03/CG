#include <algorithm>
#include "box.hpp"
#include <vector>

Primitive generateBox(float size, int divisions) {
    Primitive box = newEmptyPrimitive();
    float step = size / divisions;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = -size / 2 + i * step;
            float z1 = -size / 2 + j * step;
            float y1 = size / 2;

            float x2 = -size / 2 + (i + 1) * step;
            float z2 = -size / 2 + j * step;
            float y2 = size / 2;

            float x3 = -size / 2 + i * step;
            float z3 = -size / 2 + (j + 1) * step;
            float y3 = size / 2;

            float x4 = -size / 2 + (i + 1) * step;
            float z4 = -size / 2 + (j + 1) * step;
            float y4 = size / 2;

            addValueList(getPontos(box), newPoint(x3, y3, z3));
            addValueList(getPontos(box), newPoint(x2, y2, z2));
            addValueList(getPontos(box), newPoint(x1, y1, z1));

            addValueList(getPontos(box), newPoint(x3, y3, z3));
            addValueList(getPontos(box), newPoint(x4, y4, z4));
            addValueList(getPontos(box), newPoint(x2, y2, z2));

            addValueList(getPontos(box), newPoint(x3, -y3, z3));
            addValueList(getPontos(box), newPoint(x1, -y1, z1));
            addValueList(getPontos(box), newPoint(x2, -y2, z2));

            addValueList(getPontos(box), newPoint(x3, -y3, z3));
            addValueList(getPontos(box), newPoint(x2, -y2, z2));
            addValueList(getPontos(box), newPoint(x4, -y4, z4));
        }
    }

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float x1 = -size / 2 + i * step;
            float y1 = -size / 2 + j * step;
            float z1 = size / 2;

            float x2 = -size / 2 + (i + 1) * step;
            float y2 = -size / 2 + j * step;
            float z2 = size / 2;

            float x3 = -size / 2 + i * step;
            float y3 = -size / 2 + (j + 1) * step;
            float z3 = size / 2;

            float x4 = -size / 2 + (i + 1) * step;
            float y4 = -size / 2 + (j + 1) * step;
            float z4 = size / 2;

            addValueList(getPontos(box), newPoint(x3, y3, -z3));
            addValueList(getPontos(box), newPoint(x2, y2, -z2));
            addValueList(getPontos(box), newPoint(x1, y1, -z1));

            addValueList(getPontos(box), newPoint(x3, y3, -z3));
            addValueList(getPontos(box), newPoint(x4, y4, -z4));
            addValueList(getPontos(box), newPoint(x2, y2, -z2));

            addValueList(getPontos(box), newPoint(x3, y3, z3));
            addValueList(getPontos(box), newPoint(x1, y1, z1));
            addValueList(getPontos(box), newPoint(x2, y2, z2));

            addValueList(getPontos(box), newPoint(x3, y3, z3));
            addValueList(getPontos(box), newPoint(x2, y2, z2));
            addValueList(getPontos(box), newPoint(x4, y4, z4));
        }
    }

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            float z1 = -size / 2 + i * step;
            float y1 = -size / 2 + j * step;
            float x1 = size / 2;

            float z2 = -size / 2 + (i + 1) * step;
            float y2 = -size / 2 + j * step;
            float x2 = size / 2;

            float z3 = -size / 2 + i * step;
            float y3 = -size / 2 + (j + 1) * step;
            float x3 = size / 2;

            float z4 = -size / 2 + (i + 1) * step;
            float y4 = -size / 2 + (j + 1) * step;
            float x4 = size / 2;

            addValueList(getPontos(box), newPoint(-x3, y3, z3));
            addValueList(getPontos(box), newPoint(-x1, y1, z1));
            addValueList(getPontos(box), newPoint(-x2, y2, z2));

            addValueList(getPontos(box), newPoint(-x3, y3, z3));
            addValueList(getPontos(box), newPoint(-x2, y2, z2));
            addValueList(getPontos(box), newPoint(-x4, y4, z4));

            addValueList(getPontos(box), newPoint(x3, y3, -z3));
            addValueList(getPontos(box), newPoint(x1, y1, -z1));
            addValueList(getPontos(box), newPoint(x2, y2, -z2));

            addValueList(getPontos(box), newPoint(x3, y3, -z3));
            addValueList(getPontos(box), newPoint(x2, y2, -z2));
            addValueList(getPontos(box), newPoint(x4, y4, -z4));
        }
    }

    return box;
}
