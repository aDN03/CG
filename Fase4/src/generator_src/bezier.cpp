#include "bezier.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// Lê o ficheiro .patch e retorna a lista de patches com os respetivos pontos de controlo
std::vector<std::vector<std::vector<float>>> readPatchesFile(const char* filePath) {
    std::vector<std::vector<std::vector<float>>> result;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Erro ao abrir o ficheiro: " + std::string(filePath));
    }

    std::string line;

    // Ler o número de patches
    if (!std::getline(file, line)) {
        throw std::runtime_error("Erro ao ler o número de patches");
    }
    int numPatches = std::stoi(line);

    // Ler os índices de cada patch
    std::vector<std::vector<int>> indicesPerPatch;
    for (int i = 0; i < numPatches; i++) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Erro ao ler os índices dos patches");
        }
        std::istringstream iss(line);
        std::vector<int> indices;
        std::string token;

        while (std::getline(iss, token, ',')) {
            indices.push_back(std::stoi(token));
        }

        if (indices.size() != 16) {
            throw std::runtime_error("Cada patch deve conter exatamente 16 índices");
        }

        indicesPerPatch.push_back(indices);
    }

    // Ler o número de pontos de controlo
    if (!std::getline(file, line)) {
        throw std::runtime_error("Erro ao ler o número de pontos de controlo");
    }
    int numControlPoints = std::stoi(line);

    // Ler os pontos de controlo
    std::vector<std::vector<float>> controlPoints;
    for (int i = 0; i < numControlPoints; i++) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Erro ao ler os pontos de controlo");
        }
        std::istringstream iss(line);
        std::vector<float> point;
        std::string token;
        while (std::getline(iss, token, ',')) {
            point.push_back(std::stof(token));
        }
        if (point.size() != 3) {
            throw std::runtime_error("Cada ponto de controlo deve ter 3 coordenadas");
        }
        controlPoints.push_back(point);
    }

    // Construir os patches (cada um com 16 pontos)
    for (const auto& indices : indicesPerPatch) {
        std::vector<std::vector<float>> patch;
        for (int idx : indices) {
            if (idx < 0 || idx >= static_cast<int>(controlPoints.size())) {
                throw std::runtime_error("Índice de ponto de controlo inválido");
            }
            patch.push_back(controlPoints[idx]);
        }
        result.push_back(patch);
    }

    return result;
}

// Gera a superfície Bezier e retorna os triângulos como Primitive
Primitive generateSurface(const char* filePath, int tessellation) {
    Primitive result = newEmptyPrimitive();
    const float delta = 1.0f / tessellation;

    const auto& patches = readPatchesFile(filePath);

    for (const auto& patch : patches) {
        for (int i = 0; i < tessellation; ++i) {
            for (int j = 0; j < tessellation; ++j) {
                float u = i * delta;
                float v = j * delta;

                float A[3], B[3], C[3], D[3];
                surfacePoint(u, v, patch, A);
                surfacePoint(u, v + delta, patch, B);
                surfacePoint(u + delta, v, patch, C);
                surfacePoint(u + delta, v + delta, patch, D);

                // Triângulo 1: C - A - B
                addPontoArr(result, C);
                addPontoArr(result, A);
                addPontoArr(result, B);

                // Triângulo 2: B - D - C
                addPontoArr(result, B);
                addPontoArr(result, D);
                addPontoArr(result, C);
            }
        }
    }

    return result;
}
