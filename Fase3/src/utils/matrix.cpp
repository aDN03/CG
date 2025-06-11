#include "matrix.hpp"
#include <cmath>
#include <vector>

void multiplyMatrices(int la, int ca, const float* A, // matriz A (la x ca)
                      int lb, int cb, const float* B, // matriz B (lb x cb)
                      float* R, int* lr = nullptr, int* cr = nullptr) { // matriz R (la x cb)
    if (ca == lb) {
        if (lr) *lr = la;
        if (cr) *cr = cb;
        for (int i = 0; i < la; i++) {
            for (int j = 0; j < cb; j++) {
                R[i * cb + j] = 0;
                for (int k = 0; k < ca; k++) {
                    R[i * cb + j] += A[i * ca + k] * B[k * cb + j];
                }
            }
        }
    }
}

void buildRotMatrix(const float* x, const float* y, const float* z, float* m) {
    m[0]  = x[0]; m[1]  = x[1]; m[2]  = x[2]; m[3]  = 0;
    m[4]  = y[0]; m[5]  = y[1]; m[6]  = y[2]; m[7]  = 0;
    m[8]  = z[0]; m[9]  = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0;    m[13] = 0;    m[14] = 0;    m[15] = 1;
}

void cross(const float* a, const float* b, float* res) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

void normalize(float* a) {
    float l = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    if (l == 0.0f) return;
    a[0] /= l;
    a[1] /= l;
    a[2] /= l;
}

float length(const float* a) {
    return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

float dot(const float* v1, const float* v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

float angleVectors(const float* v1, const float* v2) {
    float dotProduct = dot(v1, v2);
    float l1 = length(v1);
    float l2 = length(v2);
    float divisor = l1 * l2;
    if (divisor == 0.0f) return 0.0f;
    return acos(std::max(-1.0f, std::min(1.0f, dotProduct / divisor))); // Clamp para evitar valores fora do domínio de acos
}

// Obtem ponto e derivada da curva Catmull-Rom
void getCatmullRomPoint(float t, const std::vector<float>& p0, const std::vector<float>& p1,
                        const std::vector<float>& p2, const std::vector<float>& p3,
                        float* pos, float* deriv) {
    float m[16] = {
        -0.5f,  1.5f, -1.5f,  0.5f,
         1.0f, -2.5f,  2.0f, -0.5f,
        -0.5f,  0.0f,  0.5f,  0.0f,
         0.0f,  1.0f,  0.0f,  0.0f
    };

    float P[12] = {
        p0[0], p0[1], p0[2],
        p1[0], p1[1], p1[2],
        p2[0], p2[1], p2[2],
        p3[0], p3[1], p3[2]
    };

    float A[12];
    multiplyMatrices(4, 4, m, 4, 3, P, A);

    float T[4] = { t*t*t, t*t, t, 1.0f };
    float DERT[4] = { 3*t*t, 2*t, 1, 0 };

    if (pos)   multiplyMatrices(1, 4, T, 4, 3, A, pos);
    if (deriv) multiplyMatrices(1, 4, DERT, 4, 3, A, deriv);
}

// Ponto e derivada global (percorrer a curva inteira com t global)
void getGlobalCatmullRomPoint(float gt, const std::vector<std::vector<float>>& controlPoints, float* pos, float* deriv) {
    size_t POINT_COUNT = controlPoints.size();
    float t = gt * POINT_COUNT;
    int index = floor(t);
    t = t - index;

    int indices[4];
    indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
    indices[1] = (indices[0] + 1) % POINT_COUNT;
    indices[2] = (indices[1] + 1) % POINT_COUNT;
    indices[3] = (indices[2] + 1) % POINT_COUNT;

    getCatmullRomPoint(t,
                       controlPoints[indices[0]],
                       controlPoints[indices[1]],
                       controlPoints[indices[2]],
                       controlPoints[indices[3]],
                       pos, deriv);
}

// Avaliação de ponto numa superfície Bezier 4x4
void surfacePoint(float u, float v, const std::vector<std::vector<float>>& patch, float* res) {
    float M[16] = {
        -1.0f,  3.0f, -3.0f, 1.0f,
         3.0f, -6.0f,  3.0f, 0.0f,
        -3.0f,  3.0f,  0.0f, 0.0f,
         1.0f,  0.0f,  0.0f, 0.0f
    };

    float U[4] = { u*u*u, u*u, u, 1.0f };
    float V[4] = { v*v*v, v*v, v, 1.0f };

    float UM[4], MV[4];
    multiplyMatrices(1, 4, U, 4, 4, M, UM);
    multiplyMatrices(4, 4, M, 4, 1, V, MV);

    float P[3][16];
    for (int k = 0; k < 3; ++k) {
        for (int i = 0; i < 16; ++i) {
            P[k][i] = patch[i][k];
        }
    }

    for (int i = 0; i < 3; ++i) {
        float UMP[4];
        multiplyMatrices(1, 4, UM, 4, 4, P[i], UMP);
        multiplyMatrices(1, 4, UMP, 4, 1, MV, &res[i]);
    }
}
