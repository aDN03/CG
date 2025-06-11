#include "ring.hpp"

Primitive generateRing(float ri, float re, int slices){
    Primitive ring = newEmptyPrimitive();
    float a = 0, delta = (2 * M_PI) / slices;
    
    if(ring){
        for (int i = 0; i < slices; i++, a += delta){
            addValueList(getPontos(ring), newPontoSph(a, 0.0f, ri));
            addValueList(getPontos(ring), newPontoSph(a, 0.0f, re));
            addValueList(getPontos(ring), newPontoSph(a + delta, 0.0f, ri));

            addValueList(getPontos(ring), newPontoSph(a + delta, 0.0f, ri));
            addValueList(getPontos(ring), newPontoSph(a, 0.0f, re));
            addValueList(getPontos(ring), newPontoSph(a + delta, 0.0f, re));

            addValueList(getPontos(ring), newPontoSph(a + delta, 0.0f, ri));
            addValueList(getPontos(ring), newPontoSph(a, 0.0f, re));
            addValueList(getPontos(ring), newPontoSph(a, 0.0f, ri));

            addValueList(getPontos(ring), newPontoSph(a, 0.0f, re));
            addValueList(getPontos(ring), newPontoSph(a + delta, 0.0f, ri));
            addValueList(getPontos(ring), newPontoSph(a + delta, 0.0f, re));
        }
    }
    
    return ring;
}



