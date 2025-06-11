#ifndef POINT
#define POINT
#include <stdlib.h>
#include <math.h>

// Structure definition for a Point, represented by a pointer to a point
typedef struct point* Point;

// Constructor

/**
 * Creates a new point with the specified coordinates (x, y, z).
 *
 * @param x The x-coordinate of the new point.
 * @param y The y-coordinate of the new point.
 * @param z The z-coordinate of the new point.
 * @return A new point with the specified coordinates.
 */
Point newPoint(float x, float y, float z);

// Getters

/**
 * Retrieves the x-coordinate of the given point.
 *
 * @param ponto The point from which the x-coordinate is to be retrieved.
 * @return The x-coordinate of the point.
 */
float getX(Point ponto);

/**
 * Retrieves the y-coordinate of the given point.
 *
 * @param ponto The point from which the y-coordinate is to be retrieved.
 * @return The y-coordinate of the point.
 */
float getY(Point ponto);

/**
 * Retrieves the z-coordinate of the given point.
 *
 * @param ponto The point from which the z-coordinate is to be retrieved.
 * @return The z-coordinate of the point.
 */
float getZ(Point ponto);

// Free Function

/**
 * Frees the memory associated with the given point.
 *
 * @param ponto The point to be freed.
 */
void freeP(Point ponto);

// Distance Calculation

/**
 * Calculates the Euclidean distance between two points.
 *
 * @param ponto1 The first point.
 * @param ponto2 The second point.
 * @return The Euclidean distance between the two points.
 */
float calculateDistance(Point ponto1, Point ponto2);

// Spherical to Cartesian Coordinate Converter

/**
 * Converts spherical coordinates (a, b, radius) to Cartesian coordinates.
 *
 * @param a The azimuthal angle in radians.
 * @param b The inclination angle in radians.
 * @param radius The radial distance from the origin.
 * @return A new point with Cartesian coordinates converted from spherical coordinates.
 */
Point newPontoSph(float a, float b, float radius);


#endif