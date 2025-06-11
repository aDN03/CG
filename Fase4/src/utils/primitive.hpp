#ifndef PRIMITIVE
#define PRIMITIVE
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "point.hpp"
#include "list.hpp"

// Structure definition for a primitive, represented by a pointer to a primitive
typedef struct primitive* Primitive;

// Constructors
Primitive newEmptyPrimitive();

/**
 * Creates a new primitive based on an existing list of points.
 *
 * @param pontos List of points to be associated with the new primitive.
 * @return A new primitive with the specified list of points.
 */
Primitive newPrimitiveFromList(List pontos);

// Getters

/**
 * Retrieves the list of points associated with the given primitive.
 *
 * @param pri The primitive whose list of points is to be retrieved.
 * @return The list of points associated with the primitive.
 */
List getPontos(Primitive pri);

/**
 * Adds points from one primitive to another.
 *
 * @param f The primitive to which points will be added.
 * @param toAdd The primitive from which points will be added.
 */
void addPontos(Primitive f, Primitive toAdd);

void addPonto(Primitive f, Point p);

void addPontoArr(Primitive f, float* p);


// Other Functions

/**
 * Writes the data of the primitive, including its list of points, to a file.
 *
 * @param pri The primitive whose data is to be written to the file.
 * @param path The path to the file where the data will be written.
 */
void primitiveToFile(Primitive pri, const char* path);

/**
 * Reads data from a file to create a new primitive, including its list of points.
 *
 * @param path The path to the file from which data will be read to create the primitive.
 * @return A new primitive with data read from the specified file.
 */
Primitive fileToPrimitive(const char* path);

/**
 * Deletes a primitive and frees the associated memory, including the list of points.
 *
 * @param pri The primitive to be deleted.
 */
void freePri(Primitive pri);


#endif