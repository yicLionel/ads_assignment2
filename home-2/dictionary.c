/*
    Data structure and function implementations for a 
    dictionary module intended to allow lookup
    for particular values.
*/
#include "dictionary.h"
#include "record_struct.h"
#include "record_struct.c"
#include "bit.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define NUMERIC_BASE 10
#define KEY_FIELD 1
#define NOTFOUND "NOTFOUND"
#define NOTDOUBLE (-1)
#define MAXPRECISION (-2)
#define NUM_FIELDS 35

#define FIELDLOOKUPFAILURE (-1)

#define INDEXINITIAL 1

// Field names by index.
static char *fieldNames[] = {
    "PFI",
    "EZI_ADD",
    "SRC_VERIF",
    "PROPSTATUS",
    "GCODEFEAT",
    "LOC_DESC",
    "BLGUNTTYP",
    "HSAUNITID",
    "BUNIT_PRE1",
    "BUNIT_ID1",
    "BUNIT_SUF1",
    "BUNIT_PRE2",
    "BUNIT_ID2",
    "BUNIT_SUF2",
    "FLOOR_TYPE",
    "FLOOR_NO_1",
    "FLOOR_NO_2",
    "BUILDING",
    "COMPLEX",
    "HSE_PREF1",
    "HSE_NUM1",
    "HSE_SUF1",
    "HSE_PREF2",
    "HSE_NUM2",
    "HSE_SUF2",
    "DISP_NUM1",
    "ROAD_NAME",
    "ROAD_TYPE",
    "RD_SUF",
    "LOCALITY",
    "STATE",
    "POSTCODE",
    "ACCESSTYPE",
    "x",
    "y"
};

/* 
Field double precision by index. -1 signifies that the value should not
be printed as a double value.
*/
static int fieldPrecision[] = {
    NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, 
    NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, 
    NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, 
    NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, 
    NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, 
    NOTDOUBLE, NOTDOUBLE, NOTDOUBLE, 5, 5
};

struct queryResult {
    char *searchString;
    int numRecords;
    double closestValue;
    struct data **records;
    int bitCount;
    int nodeCount;
    int stringCount;
};

/* CSV records. */
struct data;
struct data {
    char *PFI;
    char *EZI_ADD;
    char *SRC_VERIF;
    char *PROPSTATUS;
    char *GCODEFEAT;
    char *LOC_DESC;
    char *BLGUNTTYP;
    char *HSAUNITID;
    char *BUNIT_PRE1;
    char *BUNIT_ID1;
    char *BUNIT_SUF1;
    char *BUNIT_PRE2;
    char *BUNIT_ID2;
    char *BUNIT_SUF2;
    char *FLOOR_TYPE;
    char *FLOOR_NO_1;
    char *FLOOR_NO_2;
    char *BUILDING;
    char *COMPLEX;
    char *HSE_PREF1;
    char *HSE_NUM1;
    char *HSE_SUF1;
    char *HSE_PREF2;
    char *HSE_NUM2;
    char *HSE_SUF2;
    char *DISP_NUM1;
    char *ROAD_NAME;
    char *ROAD_TYPE;
    char *RD_SUF;
    char *LOCALITY;
    char *STATE;
    char *POSTCODE;
    char *ACCESSTYPE;
    long double x;
    long double y;
};

/* A node in the dictionary, used to allow quick lookup. */
struct dictionaryNode;

struct dictionaryNode{
    struct data *record;
    struct dictionaryNode *next;
};

struct index;

struct index {
    int valueCount;
    struct dictionaryNode **nodes;
};

/* Dictionary - concrete data structure is linked list. */
struct dictionary {
    struct dictionaryNode *head;
    struct dictionaryNode *tail;
    struct index **indices;
};

/* Reads a given string as an integer and returns the integer. */
int readIntField(char *fieldString);

/* Reads a given string as a string and returns a copy of the string. */
char *readStringField(char *fieldString);

/* Reads a given string as a double value and returns the double value. */
double readDoubleField(char *fieldString);

int readIntField(char *fieldString){
    return strtol(fieldString, NULL, NUMERIC_BASE);
}

char *readStringField(char *fieldString){
    char *str = strdup(fieldString);
    assert(str);
    return str;
}

double readDoubleField(char *fieldString){
    return strtod(fieldString, NULL);
}

/* Sets the relevant field. */
void setField(struct data *record, int fieldIndex, char *fieldVal);

void setField(struct data *record, int fieldIndex, char *fieldVal){
    switch(fieldIndex){
        case 0:
            record->PFI = readStringField(fieldVal);
            break;
        case 1:
            record->EZI_ADD = readStringField(fieldVal);
            break;
        case 2:
            record->SRC_VERIF = readStringField(fieldVal);
            break;
        case 3:
            record->PROPSTATUS = readStringField(fieldVal);
            break;
        case 4:
            record->GCODEFEAT = readStringField(fieldVal);
            break;
        case 5:
            record->LOC_DESC = readStringField(fieldVal);
            break;
        case 6:
            record->BLGUNTTYP = readStringField(fieldVal);
            break;
        case 7:
            record->HSAUNITID = readStringField(fieldVal);
            break;
        case 8:
            record->BUNIT_PRE1 = readStringField(fieldVal);
            break;
        case 9:
            record->BUNIT_ID1 = readStringField(fieldVal);
            break;
        case 10:
            record->BUNIT_SUF1 = readStringField(fieldVal);
            break;
        case 11:
            record->BUNIT_PRE2 = readStringField(fieldVal);
            break;
        case 12:
            record->BUNIT_ID2 = readStringField(fieldVal);
            break;
        case 13:
            record->BUNIT_SUF2 = readStringField(fieldVal);
            break;
        case 14:
            record->FLOOR_TYPE = readStringField(fieldVal);
            break;
        case 15:
            record->FLOOR_NO_1 = readStringField(fieldVal);
            break;
        case 16:
            record->FLOOR_NO_2 = readStringField(fieldVal);
            break;
        case 17:
            record->BUILDING = readStringField(fieldVal);
            break;
        case 18:
            record->COMPLEX = readStringField(fieldVal);
            break;
        case 19:
            record->HSE_PREF1 = readStringField(fieldVal);
            break;
        case 20:
            record->HSE_NUM1 = readStringField(fieldVal);
            break;
        case 21:
            record->HSE_SUF1 = readStringField(fieldVal);
            break;
        case 22:
            record->HSE_PREF2 = readStringField(fieldVal);
            break;
        case 23:
            record->HSE_NUM2 = readStringField(fieldVal);
            break;
        case 24:
            record->HSE_SUF2 = readStringField(fieldVal);
            break;
        case 25:
            record->DISP_NUM1 = readStringField(fieldVal);
            break;
        case 26:
            record->ROAD_NAME = readStringField(fieldVal);
            break;
        case 27:
            record->ROAD_TYPE = readStringField(fieldVal);
            break;
        case 28:
            record->RD_SUF = readStringField(fieldVal);
            break;
        case 29:
            record->LOCALITY = readStringField(fieldVal);
            break;
        case 30:
            record->STATE = readStringField(fieldVal);
            break;
        case 31:
            record->POSTCODE = readStringField(fieldVal);
            break;
        case 32:
            record->ACCESSTYPE = readStringField(fieldVal);
            break;
        case 33:
            record->x = readDoubleField(fieldVal);
            break;
        case 34:
            record->y = readDoubleField(fieldVal);
            break;
        default:
            fprintf(stderr, "%d: Unhandled field number %d\n", __LINE__, 
                fieldIndex);
            assert(fieldIndex >= 0 && fieldIndex < NUM_FIELDS);
    }
}

/* Returns the double value for the given field index. */
double getDoubleVal(struct data *record, int fieldIndex);

double getDoubleVal(struct data *record, int fieldIndex){
    switch(fieldIndex){
        case 33:
            return record->x;
        case 34:
            return record->y;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
            fprintf(stderr, "%d: Unhandled non-double field number %d\n", __LINE__, 
                fieldIndex);
            assert(0);
            break;
        default:
            fprintf(stderr, "%d: Unhandled field number %d\n", __LINE__, 
                fieldIndex);
            assert(fieldIndex >= 0 && fieldIndex < NUM_FIELDS && 0);
    }
    return FIELDLOOKUPFAILURE;
}

/* Prints a given value. */
void printIntField(FILE *f, int value);

/* Prints a given value. */
void printStringField(FILE *f, char *value);

/* Prints a given value. */
void printDoubleField(FILE *f, double value, int fieldPrecision);

void printIntField(FILE *f, int value){
    fprintf(f, "%d", value);
}

void printStringField(FILE *f, char *value){
    fprintf(f, "%s", value);
}

void printDoubleField(FILE *f, double value, int fieldPrecision){
    fprintf(f, "%.*f", fieldPrecision, value);
}

/* Prints the relevant field. */
void printField(FILE *f, struct data *record, int fieldIndex);

void printField(FILE *f, struct data *record, int fieldIndex){
    switch(fieldIndex){
        case 0:
            printStringField(f, record->PFI);
            break;
        case 1:
            printStringField(f, record->EZI_ADD);
            break;
        case 2:
            printStringField(f, record->SRC_VERIF);
            break;
        case 3:
            printStringField(f, record->PROPSTATUS);
            break;
        case 4:
            printStringField(f, record->GCODEFEAT);
            break;
        case 5:
            printStringField(f, record->LOC_DESC);
            break;
        case 6:
            printStringField(f, record->BLGUNTTYP);
            break;
        case 7:
            printStringField(f, record->HSAUNITID);
            break;
        case 8:
            printStringField(f, record->BUNIT_PRE1);
            break;
        case 9:
            printStringField(f, record->BUNIT_ID1);
            break;
        case 10:
            printStringField(f, record->BUNIT_SUF1);
            break;
        case 11:
            printStringField(f, record->BUNIT_PRE2);
            break;
        case 12:
            printStringField(f, record->BUNIT_ID2);
            break;
        case 13:
            printStringField(f, record->BUNIT_SUF2);
            break;
        case 14:
            printStringField(f, record->FLOOR_TYPE);
            break;
        case 15:
            printStringField(f, record->FLOOR_NO_1);
            break;
        case 16:
            printStringField(f, record->FLOOR_NO_2);
            break;
        case 17:
            printStringField(f, record->BUILDING);
            break;
        case 18:
            printStringField(f, record->COMPLEX);
            break;
        case 19:
            printStringField(f, record->HSE_PREF1);
            break;
        case 20:
            printStringField(f, record->HSE_NUM1);
            break;
        case 21:
            printStringField(f, record->HSE_SUF1);
            break;
        case 22:
            printStringField(f, record->HSE_PREF2);
            break;
        case 23:
            printStringField(f, record->HSE_NUM2);
            break;
        case 24:
            printStringField(f, record->HSE_SUF2);
            break;
        case 25:
            printStringField(f, record->DISP_NUM1);
            break;
        case 26:
            printStringField(f, record->ROAD_NAME);
            break;
        case 27:
            printStringField(f, record->ROAD_TYPE);
            break;
        case 28:
            printStringField(f, record->RD_SUF);
            break;
        case 29:
            printStringField(f, record->LOCALITY);
            break;
        case 30:
            printStringField(f, record->STATE);
            break;
        case 31:
            printStringField(f, record->POSTCODE);
            break;
        case 32:
            printStringField(f, record->ACCESSTYPE);
            break;
        case 33:
            printDoubleField(f, record->x, fieldPrecision[fieldIndex]);
            break;
        case 34:
            printDoubleField(f, record->y, fieldPrecision[fieldIndex]);
            break;
        default:
            fprintf(stderr, "%d: Unhandled field number %d\n", __LINE__, 
                fieldIndex);
            assert(fieldIndex >= 0 && fieldIndex < NUM_FIELDS);
    }
}

struct dictionary *newDict(){
    struct dictionary *ret = (struct dictionary *) 
        malloc(sizeof(struct dictionary));
    assert(ret);
    ret->head = NULL;
    ret->tail = NULL;
    ret->indices = NULL;
    return ret;
}

/* Read a record into a typed data structure. */
struct data *readRecord(struct csvRecord *record);

struct data *readRecord(struct csvRecord *record){
    struct data *ret = (struct data *) malloc(sizeof(struct data));
    assert(ret);
    assert(record->fieldCount == NUM_FIELDS);
    for(int i = 0; i < NUM_FIELDS; i++){
        setField(ret, i, record->fields[i]);
    }
    return ret;
};

void insertRecord(struct dictionary *dict, struct csvRecord *record){
    if(! dict){
        return;
    }
    struct dictionaryNode *newNode = (struct dictionaryNode *) 
        malloc(sizeof(struct dictionaryNode));
    assert(newNode);
    newNode->record = readRecord(record);
    newNode->next = NULL;

    if(! (dict->head)){
        /* First insertion, insert new node as head and tail. */
        dict->head = newNode;
        dict->tail = newNode;
    } else {
        /* Insert at tail. */
        dict->tail->next = newNode;
        dict->tail = newNode;
    }
}

/* Search for a given key in the dictionary. */
struct queryResult *lookupRecord(struct dictionary *dict, char *query){
    int numRecords = 0;
    struct data **records = NULL;
    int bitCount = 0;
    int nodeCount = 0;
    int stringCount = 0;
    int queryBitCount = (strlen(query) + 1) * BITS_PER_BYTE;

    /* Iterate over all records and collect all matching records. */
    struct dictionaryNode *current = dict->head;
    while(current){
        /* One string is stored per node, so these are equivalent. */
        nodeCount++;
        stringCount++;

        int nodeBitCount = (strlen(current->record->EZI_ADD) + 1) * BITS_PER_BYTE;

        for(int i = 0; i <= nodeBitCount && i <= queryBitCount; i++){
            if(i == queryBitCount && i == nodeBitCount){
                /* Match. */
                records = (struct data **) realloc(records, 
                    sizeof(struct data *) * (numRecords + 1));
                assert(records);
                records[numRecords] = current->record;
                numRecords++;
                break;
            } else if(i == queryBitCount || i == nodeBitCount){
                /* Reached end of one of the strings. */
                break;
            } else {
                /* Bit compared, regardless of success. */
                bitCount++;
                if(getBit(query, i) != getBit(current->record->EZI_ADD, i)){
                    break;
                }
            }
        }
        current = current->next;
    }

    struct queryResult *qr = (struct queryResult *) 
        malloc(sizeof(struct queryResult));
    assert(qr);

    qr->searchString = strdup(query);
    assert(qr->searchString);
    qr->numRecords = numRecords;
    qr->records = records;
    qr->bitCount = bitCount;
    qr->nodeCount = nodeCount;
    qr->stringCount = stringCount;
    return qr;
}

/* Output the given query result. */
void printQueryResult(struct queryResult *r, FILE *summaryFile, 
    FILE *outputFile, int stage){
    /* Print summary. */
    if(stage == LOOKUPSTAGE){
        if(r->numRecords == 0){
            fprintf(summaryFile, "%s --> %s\n", r->searchString, NOTFOUND);
        } else {
            fprintf(summaryFile, "%s --> %d records found - comparisons:"
                " b%d n%d s%d\n", r->searchString, r->numRecords, r->bitCount, r->nodeCount, r->stringCount);
        }
    } else if(stage == BINARYOUTPUTSTAGE) {
        if(r->numRecords == 0){
            fprintf(summaryFile, "%s --> %s\n", r->searchString, NOTFOUND);
        } else {
            fprintf(summaryFile, "%s (", r->searchString);
            for(int i = 0; i < (strlen(r->searchString) + 1) * BITS_PER_BYTE; i++) {
                if(i > 0 && (i % BITS_PER_BYTE == 0)) {
                    fprintf(summaryFile, " ");
                }
                fprintf(summaryFile, "%d", getBit(r->searchString, i));
            }
            fprintf(summaryFile, ") --> %d records found - comparisons:"
                " b%d n%d s%d\n", r->numRecords, r->bitCount, r->nodeCount, r->stringCount);
        }
    } else {
        exit(EXIT_FAILURE);
    }
    /* Print details. */
    fprintf(outputFile, "%s\n", r->searchString);
    for(int i = 0; i < r->numRecords; i++){
        fprintf(outputFile, "--> ");
        for(int j = 0; j < NUM_FIELDS; j++){
            fprintf(outputFile, "%s: ", fieldNames[j]);
            printField(outputFile, r->records[i], j);
            fprintf(outputFile, " || ");
        }
        fprintf(outputFile, "\n");
    }
}

/* Builds an index for the given (double) field in the data structure. */
struct index *buildDoubleIndex(struct dictionary *dict, int fieldIndex);

struct index *buildDoubleIndex(struct dictionary *dict, int fieldIndex){
    struct index *index = (struct index *) malloc(sizeof(struct index));
    assert(index);
    int valueCount = 0;
    struct dictionaryNode **nodes = NULL;
    
    int nodeSpace = 0;

    nodes = (struct dictionaryNode **) malloc(sizeof(struct dictionaryNode *) * INDEXINITIAL);
    assert(nodes);
    nodeSpace = INDEXINITIAL;

    /* Insertion sort. */
    struct dictionaryNode *current = dict->head;

    while(current){
        if(valueCount == nodeSpace){
            nodeSpace *= 2;
            nodes = (struct dictionaryNode **) realloc(nodes, 
                sizeof(struct dictionaryNode *) * nodeSpace);
            assert(nodes);
        }
        nodes[valueCount] = current;
        for(int j = valueCount - 1; j >= 0; j--){
            if(getDoubleVal(current->record, fieldIndex) < getDoubleVal(nodes[j]->record, fieldIndex)){
                nodes[j + 1] = nodes[j];
                nodes[j] = current;
            } else {
                /* Sorted. */
                break;
            }
        }
        valueCount++;
        current = current->next;
    }

    index->valueCount = valueCount;
    index->nodes = nodes;

    return index;
}

/* Searches for the closest value using the index built for the field. If an index
    has not yet been built, it is  */
struct queryResult *searchClosestDouble(struct dictionary *dict, char *query, 
    int fieldIndex){
    if(! dict->indices){
        dict->indices = (struct index **) malloc(sizeof(struct index *) * NUM_FIELDS);
        assert(dict->indices);
        for(int i = 0; i < NUM_FIELDS; i++){
            (dict->indices)[i] = NULL;
        }
    }
    if(! dict->indices[fieldIndex]){
        dict->indices[fieldIndex] = buildDoubleIndex(dict, fieldIndex);
    }

    double searchVal = strtod(query, NULL);

    struct index *index = dict->indices[fieldIndex];
    int highIdx = index->valueCount - 1;
    int lowIdx = 0;
    while(highIdx > lowIdx){
        int mid = (highIdx + lowIdx) / 2;
        if(getDoubleVal(index->nodes[mid]->record, fieldIndex) < searchVal){
            lowIdx = mid + 1;
        } else if (getDoubleVal(index->nodes[mid]->record, fieldIndex) > searchVal){
            highIdx = mid - 1;
        } else {
            // Exact match. Terminate early.
            lowIdx = mid;
            highIdx = mid;
        }
    }
    int currentIdx = (highIdx + lowIdx) / 2;
    if(currentIdx < 0){
        currentIdx = 0;
    } else if(currentIdx >= index->valueCount){
        currentIdx = index->valueCount - 1;
    }
    double closestProximity = getDoubleVal(index->nodes[currentIdx]->record, fieldIndex) - searchVal;
    /* Check surrounding values */
    if(closestProximity < 0){
        /* searchVal is larger than the current index. */
        closestProximity = -closestProximity;
        /* Check if value higher is closer. */
        if((currentIdx + 1) < index->valueCount){
            double candidateProximity = getDoubleVal(index->nodes[currentIdx + 1]->record, fieldIndex) 
                - searchVal;
            if(candidateProximity < 0){
                candidateProximity = -candidateProximity;
            }
            if(candidateProximity < closestProximity){
                currentIdx = currentIdx + 1;
            }
        }
    } else {
        /* searchVal is smaller or equal to the current index. */
        if((currentIdx - 1) >= 0){
            double candidateProximity = getDoubleVal(index->nodes[currentIdx - 1]->record, fieldIndex) 
                - searchVal;
            if(candidateProximity < 0){
                candidateProximity = -candidateProximity;
            }
            if(candidateProximity < closestProximity){
                currentIdx = currentIdx - 1;
            }
        }
    }
    
    struct queryResult *qr = (struct queryResult *) 
        malloc(sizeof(struct queryResult));
    assert(qr);

    int numRecords = 1;

    struct data **records = (struct data **) 
        malloc(sizeof(struct data *) * (numRecords));
    assert(records);

    records[0] = index->nodes[currentIdx]->record;

    double closestValue = getDoubleVal(index->nodes[currentIdx]->record, fieldIndex);

    qr->searchString = strdup(query);
    assert(qr->searchString);
    qr->numRecords = numRecords;
    qr->records = records;
    qr->closestValue = closestValue;
    
    return qr;
}


/* Free the given query result. */
void freeQueryResult(struct queryResult *r){
    if(! r){
        return;
    }
    free(r->records);
    free(r->searchString);
    free(r);
}

void freeData(struct data *d){
    if(! d){
        return;
    }
    if (d->PFI){
        free(d->PFI);
    }
    if (d->EZI_ADD){
        free(d->EZI_ADD);
    }
    if (d->SRC_VERIF){
        free(d->SRC_VERIF);
    }
    if (d->PROPSTATUS){
        free(d->PROPSTATUS);
    }
    if (d->GCODEFEAT){
        free(d->GCODEFEAT);
    }
    if (d->LOC_DESC){
        free(d->LOC_DESC);
    }
    if (d->BLGUNTTYP){
        free(d->BLGUNTTYP);
    }
    if (d->HSAUNITID){
        free(d->HSAUNITID);
    }
    if (d->BUNIT_PRE1){
        free(d->BUNIT_PRE1);
    }
    if (d->BUNIT_ID1){
        free(d->BUNIT_ID1);
    }
    if (d->BUNIT_SUF1){
        free(d->BUNIT_SUF1);
    }
    if (d->BUNIT_PRE2){
        free(d->BUNIT_PRE2);
    }
    if (d->BUNIT_ID2){
        free(d->BUNIT_ID2);
    }
    if (d->BUNIT_SUF2){
        free(d->BUNIT_SUF2);
    }
    if (d->FLOOR_TYPE){
        free(d->FLOOR_TYPE);
    }
    if (d->FLOOR_NO_1){
        free(d->FLOOR_NO_1);
    }
    if (d->FLOOR_NO_2){
        free(d->FLOOR_NO_2);
    }
    if (d->BUILDING){
        free(d->BUILDING);
    }
    if (d->COMPLEX){
        free(d->COMPLEX);
    }
    if (d->HSE_PREF1){
        free(d->HSE_PREF1);
    }
    if (d->HSE_NUM1){
        free(d->HSE_NUM1);
    }
    if (d->HSE_SUF1){
        free(d->HSE_SUF1);
    }
    if (d->HSE_PREF2){
        free(d->HSE_PREF2);
    }
    if (d->HSE_NUM2){
        free(d->HSE_NUM2);
    }
    if (d->HSE_SUF2){
        free(d->HSE_SUF2);
    }
    if (d->DISP_NUM1){
        free(d->DISP_NUM1);
    }
    if (d->ROAD_NAME){
        free(d->ROAD_NAME);
    }
    if (d->ROAD_TYPE){
        free(d->ROAD_TYPE);
    }
    if (d->RD_SUF){
        free(d->RD_SUF);
    }
    if (d->LOCALITY){
        free(d->LOCALITY);
    }
    if (d->STATE){
        free(d->STATE);
    }
    if (d->POSTCODE){
        free(d->POSTCODE);
    }
    if (d->ACCESSTYPE){
        free(d->ACCESSTYPE);
    }
    free(d);
}

/* Free a given dictionary. */
void freeDict(struct dictionary *dict){
    if(! dict){
        return;
    }
    struct dictionaryNode *current = dict->head;
    struct dictionaryNode *next;
    while(current){
        next = current->next;
        freeData(current->record);
        free(current);
        current = next;
    }
    if(dict->indices){
        for(int i = 0; i < NUM_FIELDS; i++){
            if(dict->indices[i]){
                if(dict->indices[i]->nodes){
                    free(dict->indices[i]->nodes);
                }
                free(dict->indices[i]);
            }
        }
        free(dict->indices);
    }
    free(dict);
}

