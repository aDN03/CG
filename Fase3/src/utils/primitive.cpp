#include "primitive.hpp"

struct primitive
{
    List pontos;
};

//construtor
Primitive newEmptyPrimitive() {
    Primitive pri = (Primitive)malloc(sizeof(struct primitive));
    if (pri) {
        pri->pontos = newL();
    }
return pri;
}

Primitive newPrimitiveFromList(List pontos) {
    Primitive pri = newEmptyPrimitive();
    List temp = pontos;
    if (pri) {
        while (temp) {
            addValueList(pri->pontos, getData(temp));
            temp = getNext(temp);
        }
    }
return pri;
}

//gets
List getPontos(Primitive pri){
    if(pri){
        return pri->pontos;
    }
return NULL;
}

void addPontos(Primitive f, Primitive toAdd){
    if(f){
        List pontos = toAdd->pontos;
        for(unsigned long i = 0; i < calculateLength(pontos); i++){
            addValueList(f->pontos,getDataByIndex(pontos,i));
        }
    }
}

void addPonto(Primitive f, Point p){
    if(f){
        addValueList(f->pontos, p);
    }
}

void addPontoArr(Primitive f, float* p){
    addPonto(f,newPoint(p[0],p[1],p[2]));
}



//others
void primitiveToFile(Primitive pri, const char* path) {
    if (!pri || !pri->pontos || !path) {
        printf("Impossivel criar ficheiro devido a argumentos inválidos\n");
        return;
    }

    FILE* file = fopen(path, "w");
    if (!file) {
        printf("Ocorreu um erro na abertura do ficheiro '%s'\n", path);
        return;
    }

    fprintf(file, "%lu\n", calculateLength(pri->pontos));

    List temp = pri->pontos;
    while (temp) {
        Point p = (Point)getData(temp);
        if (p) {
            fprintf(file, "%.3f,%.3f,%.3f\n", getX(p), getY(p), getZ(p));
        }
        temp = getNext(temp);
    }

    fclose(file);
}

Primitive fileToPrimitive(const char* path) {
    Primitive pri = newEmptyPrimitive();
    FILE* file = fopen(path, "r");

    if (!pri || !file) {
        printf("Impossível ler o arquivo '%s'\n", path);
        return pri;
    }

    int vertices;
    if (fscanf(file, "%d", &vertices) != 1) {
        printf("Formato do arquivo inválido\n");
        fclose(file);
        return pri;
    }

    float x, y, z;
    for (int i = 0; i < vertices; i++) {
        if (fscanf(file, "%f,%f,%f", &x, &y, &z) == 3) {
            addValueList(pri->pontos, newPoint(x, y, z));
        } else {
            printf("Formato de dados inválido na linha %d\n", i + 2);
        }
    }

    fclose(file);
    return pri;
}

void freePri(Primitive pri) {
    if (!pri || !pri->pontos) {
        printf("Impossível deletar a primitiva devido a argumentos inválidos\n");
        return;
    }

    List temp = pri->pontos;
    while (temp) {
        freeP((Point)getData(temp));
        temp = getNext(temp);
    }

    freeL(pri->pontos);
    pri->pontos = NULL;

    free(pri);
}



