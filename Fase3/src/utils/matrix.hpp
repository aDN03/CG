#include <stdlib.h>
#include <math.h>
#include <vector>

using namespace std;

/**
 * Calcula a multiplicação entre duas matrizes.
 * @param la Número de linhas da matriz A
 * @param ca Número de colunas da matriz A
 * @param A Matriz envolvida na multiplicação
 * @param lb Número de linhas da matriz A
 * @param cb Número de colunas da matriz A
 * @param B Matriz envolvida na multiplicação
 * @param R Matriz com o resultado da multiplicação.
 * @param lr Número de linhas da matriz R (parâmetro opcional)
 * @param cr Número de colunas da matriz R (parâmetro opcional)
*/ 
void multiplyMatrices(int, int, const float*, int, int, const float*, float*, int*, int*);

/**
 * Constrói a matriz de rotação com base dos eixos X, Y e Z do sistema de coordenadas.  
 * @param x Eixo X do sistema de coordenadas.
 * @param y Eixo Y do sistema de coordenadas.
 * @param z Eixo Z do sistema de coordenadas.
 * @param m Matriz de rotação resultante.
*/
void buildRotMatrix(const float *x, const float *y, const float *z, float *m);

/**
 * Calcula o producto cruzado (cross product) de dois vectores.
 * @param a Vector que é utilizado no cálculo
 * @param b Vector que é utilizado no cálculo
 * @param res Vector que armazena no resultado do producto.
*/
void cross(const float *a, const float *b, float *res);

/**
 * Normaliza um vector
 * @param a Vector a ser normalizado
*/
void normalize(float *a);

/**
 * Calcula a norma de um vector.
 * @param a Vector cuja norma vai ser calculada.
*/
float length(float *a);

/**
 * Calcula o produto escalar entre dois vectores.
*/
float dot(float* v1, float* v2);

/**
 * Calcula o ângulo entre dois vectores.
*/
float angleVectors(float* v1, float* v2);

/**
 * Esta função é aquela p(t)
 * @param t instante de tempo da função p(t)
 * @param p0 Ponto de controlo da curva
 * @param p1 Ponto de controlo da curva
 * @param p2 Ponto de controlo da curva
 * @param p3 Ponto de controlo da curva
 * @param pos Ponto resultante (se não quiser calcular isto, colocar NULL no argumento)
 * @param deriv Derivada resultante (se não quiser calcular isto, colocar NULL no argumento)
*/
void getCatmullRomPoint(float t, const std::vector<float>& p0, const std::vector<float>& p1, const std::vector<float>& p2, const std::vector<float>& p3, float* pos, float* deriv);

void getGlobalCatmullRomPoint(float gt, const std::vector<std::vector<float>>& controlPoints, float* pos, float* deriv);


void surfacePoint(float u, float v, const std::vector<std::vector<float>>& patch, float* res);
