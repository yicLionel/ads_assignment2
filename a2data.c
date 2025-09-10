/* 
 * Assignment 2 data structure implementation
 * Handles conversion and memory management for address data
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "a2data.h"

/* Convert CSV record to a2_data structure */
a2_data *a2_from_csvRecord(struct csvRecord *rec){
    a2_data *d = malloc(sizeof(a2_data));
    assert(d);
    
    for(int i = 0; i < A2_NUM_FIELDS; i++){
        d->fields[i] = strdup(rec->fields[i]);
        assert(d->fields[i]);
    }
    return d;
}

/* Free a2_data structure and all associated memory */
void a2_free(a2_data *d){
    if(!d) return;
    
    for(int i = 0; i < A2_NUM_FIELDS; i++){
        free(d->fields[i]);
    }
    free(d);
}
