/* 
    Data structure and function implementations for a 
    csv parsing module intended to convert from CSV files
    into C strings.
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "read.h"
#include "record_struct.h"
#include "record_struct.c"

#define INIT_RECORDS 1
#define NUM_FIELDS 35

/* 
Reads a line - removing trailing whitespace, returns a csvRecord or NULL
if parsing was unsuccessful. 
*/
struct csvRecord *parseLine(char *line);

/*
Uses the parsing structure to see if the next line from the CSV needs to
be included.
*/
void checkLine(char **line, size_t *size, FILE *csvFile, char **line2, size_t *size2);

struct csvRecord **readCSV(FILE *csvFile, int *n){
    struct csvRecord **records = NULL;
    int numRecords = 0;
    int spaceRecords = 0;

    char *line = NULL;
    size_t size = 0;

    /* 
    We want to drop the header as the reading does not have that
    functionality.
    */
    int droppedFirstLine = 0;

    /* Used to speed up checkLine function. */
    char *line2 = NULL;
    size_t size2 = 0;

    while(getline(&line, &size, csvFile) > 0){
        if(! droppedFirstLine){
            droppedFirstLine = 1;
            continue;
        }
        if(spaceRecords == 0){
            records = (struct csvRecord **) 
                malloc(sizeof(struct csvRecord *) * INIT_RECORDS);
            assert(records);
            spaceRecords = INIT_RECORDS;
        } else if(numRecords >= spaceRecords){
            spaceRecords *= 2;
            records = (struct csvRecord **)
                realloc(records, sizeof(struct csvRecord *) * spaceRecords);
            assert(records);
        }
        /* If the line ends in an open double quote, we may need to extend the 
            line. */
        checkLine(&line, &size, csvFile, &line2, &size2);
        records[numRecords] = parseLine(line);
        if(records[numRecords]){
            numRecords++;
        }
    }
    if(line){
        free(line);
    }
    if(line2){
        free(line2);
    }

    /* Shrink. */
    records = (struct csvRecord **)
                realloc(records, sizeof(struct csvRecord *) * numRecords);
    assert(records);

    *n = numRecords;
    return records;
}

void checkLine(char **line, size_t *size, FILE *csvFile, char **line2, size_t *size2) {
    // char *line2 = NULL;
    // size_t size2 = 0;
    int lineLen = strlen(*line);
    int progress = 0;
    int inQuotes = 0;
    while(progress <= lineLen) {
        if((*line)[progress] == '\"'){
            /* Flip inQuotes value. */
            if(inQuotes){
                inQuotes = 0;
            } else {
                inQuotes = 1;
            }
        } else if ((*line)[progress] == '\0'){
            /* If in quotes, include following line, otherwise, end of 
                string. */
            if(inQuotes){
                /* CSV is malformed if there is not an end quote. */
                assert(getline(line2, size2, csvFile) > 0);
                size_t line2Len = strlen(*line2);
                if((lineLen + line2Len + 1) > *size){
                    /* line needs more space. */
                    *line = (char *) realloc(*line, sizeof(char) * (lineLen + line2Len + 1));
                    assert(*line);
                }
                strcat(*line, *line2);
                lineLen = strlen(*line);
                /* \0 is deleted from *line, so we need to decrement progress by one. */
                progress--;
            }
        }
        progress++;
    }
}

/* This function reads CSVs. */
struct csvRecord *parseLine(char *line){
    struct csvRecord *ret = NULL;
    char **fields = (char **) malloc(sizeof(char *) * NUM_FIELDS);
    assert(fields);
    int fieldNum = 0;
    int len = strlen(line);
    /* Remove trailing whitespace first. */
    while(len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')){
        line[len - 1] = '\0';
        len--;
    }
    /* Check for empty lines. */
    if(len == 0){
        return NULL;
    }

    int progress = 0;
    int start = 0;
    /* For simplicity assume quotes only escape comma fields. */
    int inQuotes = 0;

    while(progress <= len){
        if(line[progress] == '\"'){
            if(inQuotes){
                inQuotes = 0;
            } else {
                inQuotes = 1;
            }
        } else if(line[progress] == ',' || line[progress] == '\0'){
            if(inQuotes){
                /* Skip over, comma is escaped. */
                /* Verify CSV not malformed. */
                assert(line[progress] != '\0');
            } else {
                /* Check fields match expected. */
                assert(line[progress] != '\0' || fieldNum == (NUM_FIELDS - 1));
                /* Terminate */
                line[progress] = '\0';
                assert(fieldNum < NUM_FIELDS);
                fields[fieldNum] = strdup(line + start);
                assert(fields[fieldNum]);
                fieldNum++;
                start = progress + 1;
            }
        }
        progress++;
    }
    /* Sanity check! Did we get everything? */
    assert(fieldNum == NUM_FIELDS);

    /* Step 2: Clean extraneous quotes. */
    for(int i = 0; i < NUM_FIELDS; i++){
        if(strlen(fields[i]) > 0){
            if(fields[i][0] == '\"'){
                assert(fields[i][strlen(fields[i]) - 1] == '\"');
                fields[i][strlen(fields[i]) - 1] = '\0';
                int len = strlen(fields[i]);
                for(int j = 0; j < len; j++){
                    // Shuffle all characters along one character.
                    fields[i][j] = fields[i][j + 1];
                }
            }
        }
    }

    /* Step 3: Reduce quote count where occuring. */
    for(int i = 0; i < NUM_FIELDS; i++){
        if(strlen(fields[i]) > 0){
            /* Progress pointer - marks how much of the string has 
                been written. */
            int progress = 0;
            for(int j = 0; j <= strlen(fields[i]); j++){
                if(fields[i][j] == '\"'){
                    /* Quotes always appear in pairs, so skip over first 
                        quote. */
                    j++;
                }
                if(j != progress){
                    fields[i][progress] = fields[i][j];
                }
                progress++;
            }
        }
    }

    ret = (struct csvRecord *) malloc(sizeof(struct csvRecord));
    assert(ret);
    ret->fieldCount = fieldNum;
    ret->fields = fields;

    return ret;
}

char *getQuery(FILE *f){
    char *line = NULL;
    size_t size = 0;
    if(getline(&line, &size, f) > 0){
        while(strlen(line) > 0 && (line[strlen(line) - 1] == '\n' 
                || line[strlen(line) - 1] == '\r')){
            line[strlen(line) - 1] = '\0';
        }
        return line;
    } else {
        if(line){
            free(line);
        }
        return NULL;
    }
}

void freeCSV(struct csvRecord **dataset, int n){
    if(! dataset){
        return;
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; j < dataset[i]->fieldCount; j++){
            free(dataset[i]->fields[j]);
        }
        free(dataset[i]->fields);
        free(dataset[i]);
    }
    free(dataset);
}

