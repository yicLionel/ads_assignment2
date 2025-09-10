/* 
 * CSV Record Structure Header
 * 
 * This header defines the fundamental data structure used for transferring
 * CSV data between different modules. It provides a generic container
 * for variable-length records with multiple fields.
 */

#ifndef RECORD_STRUCT_H
#define RECORD_STRUCT_H

/* 
 * Generic CSV record structure
 * Used as an intermediate format when reading CSV files
 * Can hold any number of fields as an array of string pointers
 */
struct csvRecord {
    int fieldCount;     // Number of fields in this record
    char **fields;      // Array of string pointers, one for each field
};

#endif