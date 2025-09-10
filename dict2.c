/*
    A dictionary which allows the lookup of a list of addresses at the command
    line using Patricia Trie for Stage 2.

    Compile with
        make dict2
    
    Run with
        ./dict2 2 <input dataset> <output file> < <keys file>
    Where
        <input dataset> is the filename of the input csv.
        <output file> is the filename of the output text file.
        <keys file> is a list of keys separated by newlines.
    
    Written for COMP20003 Assignment 2 - Stage 2
    Uses Patricia Trie for efficient exact and approximate string matching
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "read.h"
#include "a2data.h"
#include "patricia.h"
#include "metrics.h"
#include "dictionary.h"

#define MINARGS 4
#define EXPECTED_STAGE "2"
#define ALT_STAGE "1"
#define STAGE (LOOKUPSTAGE)
#define STAGE2 ()

int main(int argc, char **argv){
    if(argc < MINARGS){
        fprintf(stderr, "Insufficient arguments, run in form:\n"
                        "\t./dict2 2 <input dataset> <output file> < <keys file>\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(EXPECTED_STAGE, argv[1]) != 0 && strcmp(ALT_STAGE, argv[1]) != 0){
        fprintf(stderr, "Expected stage was %s, received %s\n", EXPECTED_STAGE, argv[1]);
        exit(EXIT_FAILURE);
    }

    char *inputCSVName = argv[2];
    char *outputFileName = argv[3];

    FILE *csvFile = fopen(inputCSVName, "r");
    assert(csvFile);
    FILE *outputFile = fopen(outputFileName, "w");
    assert(outputFile);
    
    int n;
    struct csvRecord **dataset = readCSV(csvFile, &n);

    ptree_t *tree = newPatriciaDict();

    for(int i = 0; i < n; i++){
        a2_data *d = a2_from_csvRecord(dataset[i]);
        const char *key = d->fields[1];  // EZI_ADD field
        insertPatriciaRecord(tree, d, key);
    }
    freeCSV(dataset, n);

    char *query = NULL;
    while((query = getQuery(stdin))){
        struct queryResult *r = lookupPatriciaRecord(tree, query);
        /* BINARYOUTPUTSTAGE outputs binary versions of the key in addition to the key */
        printQueryResult(r, stdout, outputFile, STAGE);
        freeQueryResult(r);
        free(query);
    }

    freePatriciaDict(tree);
    tree = NULL;

    fclose(csvFile);
    fclose(outputFile);

    return EXIT_SUCCESS;
}
