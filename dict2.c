/* 
 * Dictionary lookup program using Patricia Trie - Stage 2
 * Supports exact and approximate string matching with edit distance
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "read.h"
#include "a2data.h"
#include "patricia.h"
#include "metrics.h"
#include "dictionary.h"

#define EXPECTED_ARGS 4
#define STAGE_EXPECTED "2"
#define KEY_FIELD 1
#define NUM_FIELDS 35
#define NOTFOUND "NOTFOUND"

/* Field names are now defined in dictionary.c */

/* Record printing is now handled by dictionary.c printQueryResult function */

/* 
 * Main function for Stage 2 dictionary lookup program
 * Usage: ./dict2 2 <input_csv> <output_file>
 * Reads queries from stdin and outputs results to the specified file
 * Uses Patricia Trie for efficient exact and approximate string matching
 */
int main(int argc, char **argv){
    /* Validate command line arguments */
    if(argc != EXPECTED_ARGS){
        fprintf(stderr, "Usage: %s 2 <input_csv> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    if(strcmp(argv[1], STAGE_EXPECTED) != 0){
        fprintf(stderr, "Stage must be 2 for dict2.\n");
        return EXIT_FAILURE;
    }
    
    /* Open input CSV file for reading */
    FILE *csv = fopen(argv[2], "r"); 
    if(!csv){ 
        perror("open csv"); 
        return EXIT_FAILURE; 
    }
    
    /* Open output file for writing results */
    FILE *out = fopen(argv[3], "w"); 
    if(!out){ 
        perror("open out"); 
        fclose(csv); 
        return EXIT_FAILURE; 
    }

    /* Read and parse the CSV data */
    int n = 0; 
    struct csvRecord **raw = readCSV(csv, &n);
    
    /* Create a new Patricia Trie dictionary using dictionary module */
    ptree_t *tree = newPatriciaDict();

    /* Insert all records into the Patricia Trie using EZI_ADD field as key */
    for(int i = 0; i < n; i++){
        a2_data *d = a2_from_csvRecord(raw[i]);
        const char *key = d->fields[KEY_FIELD];  // KEY_FIELD = 1 (EZI_ADD)
        insertPatriciaRecord(tree, d, key);
    }
    freeCSV(raw, n);  // Free the raw CSV data as it's now stored in the trie

    /* Process queries from standard input until EOF */
    char *query = NULL;
    while((query = getQuery(stdin))){
        /* Use dictionary module for Patricia Trie lookup */
        struct queryResult *r = lookupPatriciaRecord(tree, query);
        
        /* Output results using dictionary module's printQueryResult function */
        printQueryResult(r, stdout, out, LOOKUPSTAGE);
        
        /* Free the query result */
        freeQueryResult(r);
        free(query);
    }

    /* Clean up resources using dictionary module */
    freePatriciaDict(tree);   // Free the Patricia Trie and all associated data
    fclose(csv);              // Close input file
    fclose(out);              // Close output file
    return EXIT_SUCCESS;
}
