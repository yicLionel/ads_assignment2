/*
    Data structure declarations and prototypes for a 
    dictionary module intended to allow lookup
    for particular values.
*/
#include "record_struct.h"
#include "patricia.h"
#include "a2data.h"
#include <stdio.h>

#define LOOKUPSTAGE 1
#define BINARYOUTPUTSTAGE 2

/* Result of a query. */
struct queryResult;

/* Dictionary. */
struct dictionary;

/* Returns an empty dictionary. */
struct dictionary *newDict();

/* Insert a given record into the dictionary. */
void insertRecord(struct dictionary *dict, struct csvRecord *record);

/* Search for a given key in the dictionary. */
struct queryResult *lookupRecord(struct dictionary *dict, char *query);

/* Search for the closest record in the dictionary to the query string in the given
    field index. Assumes the field selected is double type. */
struct queryResult *searchClosestDouble(struct dictionary *dict, char *query, 
    int fieldIndex);

/* Output the given query result. 
    If stage is LOOKUPSTAGE, information retrieved by the lookupRecord
        function can be printed.
    
    The formats of these print outs is described in the COMP20003 Assignment 1 
        specification where summaryFile refers to stdout and outputFile refers 
        to the output file.
*/
void printQueryResult(struct queryResult *r, FILE *summaryFile, 
    FILE *outputFile, int stage);

/* Free the given query result. */
void freeQueryResult(struct queryResult *r);

/* Free a given dictionary. */
void freeDict(struct dictionary *dict);

/* Patricia Trie specific functions for Stage 2 */

/* Create a new Patricia Trie dictionary. */
ptree_t *newPatriciaDict();

/* Insert a record into the Patricia Trie dictionary. */
void insertPatriciaRecord(ptree_t *dict, a2_data *record, const char *key);

/* Search for records in Patricia Trie with exact and approximate matching. */
struct queryResult *lookupPatriciaRecord(ptree_t *dict, char *query);

/* Free a Patricia Trie dictionary. */
void freePatriciaDict(ptree_t *dict);

/* Print a single field of an a2_data record. */
void printA2Field(FILE *f, a2_data *record, int fieldIndex);

