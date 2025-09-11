/*
   main.c
   driver for Assignment 1: using linked list for dictionary
                              operations supplied; insert, search, deleted
     Compile with:
            make
     Usage: to see usage, run:
           ./dict1

     Written by Anh Vo for COMP20003 as a sample solution
        for Assignment 1, Semester 2 2024
     Adapted by Grady Fitzpatrick as a sample solution
        for Assignment 1, Semester 2 2025
     Last updated: 04/07/2025
*/


#include "utils.h"
#include "dict.h"
#include "data.h"

// valid tasks
typedef enum {
    LOWER_TASK = 0, // bound
    LL_SEARCH = 1,  // search-on-linked-list
    // By default, enum values take the value preceeding
    //  plus one.
    UPPER_TASK      // bound
} task_t;

#define ARGC 4

void processArgs(int argc, char *argv[], task_t *, char **, char **);
void batchSearch(dict_t *dict, char *outFileName, FILE *msgFile);
void batchDelete(dict_t *dict, char *outFileName, FILE *msgFile);


int main(int argc, char *argv[]) {
    task_t task;
    char *inFileName, *outFileName;
    processArgs(argc, argv, &task, &inFileName, &outFileName);
    concreteDS_t dsType = LINKED_LIST;

    FILE *msgFile = stdout;
    // Build linked list dictionary from csv file argv[2]
    FILE *inFile = myFopen(inFileName, "r");
    char **labels = dataGetLabels(inFile);            // Read CSV header line
    dict_t *dict = dictCreate(dsType, TRUE, labels);  // Create empty dict
    void *data = NULL;
    while ( (data = dataGetLine(inFile)) != NULL)  {
        dictInsert(dict, data);                        // Build the dataset
    }
    fclose(inFile);


    // do respective task
    switch (task) {
        case LL_SEARCH:
            batchSearch(dict, outFileName, msgFile);
            break;
        default:
            error("main: Unrecognized Task");
    }
    // top-level cleaning
    dictFree(dict);
    return 0;
}


// Do multiple search on dict, output result after each search
// note: dict can be of any types (concrete data structures)
void batchSearch(dict_t *dict, char *outFileName, FILE *msgFile) {
    FILE *outFile = myFopen(outFileName, "w");
    char *query = NULL;
    while ( (query = getString(stdin)) != NULL) {
        // create an empty linked list dictionary for holding search output
        dict_t *outputDict = dictCopyStructure(dict);
        // perform a search, then output
        int matches = dictSearch(query, dict, outputDict);
        if (matches){
            fprintf(msgFile, "%s --> %d records found\n", query, matches);
        } else {
            fprintf(msgFile, "%s --> NOTFOUND\n", query);
        }

        fprintf(outFile, "%s -->\n", query);
        dictPrint(outputDict, outFile);
        // cleaning after each query
        dictFree(outputDict);
        free(query);
    }
    fclose(outFile);
}

// check arguments of main()
void processArgs(int argc, char *argv[], task_t *task, char **inFileName, char **outFileName) {
    if (argc < ARGC || strtol(argv[1], NULL, 10) <= (int) LOWER_TASK
                    || strtol(argv[1], NULL, 10) >= (int) UPPER_TASK)  {
        fprintf(stderr, "\n    ---- USING A DICTIONARY OF AUSTRALIAN SUBURBS ----\n");
        fprintf(stderr, "This program perform search/delete operations on a dictionary.\n");
        fprintf(stderr, "The dictionary is built from a CSV file about Australian suburbs.\n\n");
        fprintf(stderr, "Usage: %s LOWER_TASK input_file output_file\n", argv[0]);
        fprintf(stderr, "       \t where:\n");
        fprintf(stderr, "       \t    - task is 1 for search\n");
        fprintf(stderr, "       \t    - input_file: input CSV data file\n");
        fprintf(stderr, "       \t    - output_file: result output file\n");
        fprintf(stderr, "Notes on input queries:\n");
        fprintf(stderr, "       each query is a suburb name in a single line\n\n");
        exit(EXIT_FAILURE);
    }
    *task = strtol(argv[1], NULL, 10);
    *inFileName = argv[2];
    *outFileName = argv[3];
    return;
}
