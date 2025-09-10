/* 
 * Assignment 2 Data Structure Header
 * 
 * This header defines the data structure used to store address records
 * for the Patricia Trie dictionary implementation. It provides a simplified
 * interface for handling CSV records with exactly 35 fields.
 */

#ifndef A2DATA_H
#define A2DATA_H

#include "record_struct.h"

/* Number of fields in each address record */
#define A2_NUM_FIELDS 35

/* 
 * Structure to hold a single address record
 * Contains all 35 fields from the CSV data as string pointers
 * Fields 0-32 contain various address components
 * Fields 33-34 contain x,y coordinates as strings
 */
typedef struct a2_data {
    char *fields[A2_NUM_FIELDS]; // Array of string pointers for all 35 fields
} a2_data;

/* 
 * Convert a generic CSV record to an a2_data structure
 * Allocates memory for the a2_data structure and copies all field strings
 * Returns: pointer to newly allocated a2_data structure
 */
a2_data *a2_from_csvRecord(struct csvRecord *rec);

/* 
 * Free an a2_data structure and all associated memory
 * Frees all field strings and the structure itself
 * Safe to call with NULL pointer
 */
void a2_free(a2_data *d);

#endif

