#ifndef LIST
#define LIST
#include <stdlib.h>

// Structure definition for a list, represented by a pointer to a list
typedef struct list* List;

// Constructor

/**
 * Creates a new empty list.
 *
 * @return A new empty list.
 */
List newL();

// Add Function

/**
 * Adds a value to the end of the list.
 *
 * @param lista The list to which the value is to be added.
 * @param data The value to be added to the list.
 */
void addValueList(List lista, void* data);

// Getters

/**
 * Retrieves the data stored in the given list node.
 *
 * @param lista The list node from which data is to be retrieved.
 * @return The data stored in the list node.
 */
void* getData(List lista);

/**
 * Retrieves the next node in the list.
 *
 * @param lista The current list node.
 * @return The next node in the list.
 */
List getNext(List lista);

/**
 * Retrieves the data stored in the list node at the specified index.
 *
 * @param lista The list from which data is to be retrieved.
 * @param index The index of the list node.
 * @return The data stored in the list node at the specified index.
 */
void* getDataByIndex(List lista, unsigned long index);

/**
 * Calculates the length (number of nodes) of the list.
 *
 * @param lista The list for which the length is to be calculated.
 * @return The length of the list.
 */
unsigned long calculateLength(List lista);

// Free Function

/**
 * Frees the memory associated with the given list, including all nodes.
 *
 * @param lista The list to be freed.
 */
void freeL(List lista);


#endif