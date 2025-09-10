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

#define EXPECTED_ARGS 4
#define STAGE_EXPECTED "2"
#define KEY_FIELD 1
#define NUM_FIELDS 35
#define NOTFOUND "NOTFOUND"

/* 
 * Field names array for formatted output of address records
 * Contains all 35 field names corresponding to the columns in the CSV data
 * Used to create human-readable output with field labels
 */
static char *field_names[] = {
    "PFI", "EZI_ADD", "SRC_VERIF", "PROPSTATUS", "GCODEFEAT", "LOC_DESC", 
    "BLGUNTTYP", "HSAUNITID", "BUNIT_PRE1", "BUNIT_ID1", "BUNIT_SUF1", 
    "BUNIT_PRE2", "BUNIT_ID2", "BUNIT_SUF2", "FLOOR_TYPE", "FLOOR_NO_1", 
    "FLOOR_NO_2", "BUILDING", "COMPLEX", "HSE_PREF1", "HSE_NUM1", "HSE_SUF1", 
    "HSE_PREF2", "HSE_NUM2", "HSE_SUF2", "DISP_NUM1", "ROAD_NAME", "ROAD_TYPE", 
    "RD_SUF", "LOCALITY", "STATE", "POSTCODE", "ACCESSTYPE", "x", "y"
};

/* 
 * Print a single address record in formatted output
 * Outputs all fields with their names in the format: "field_name: value ||"
 * Each record starts with "-->" and ends with a newline
 */
static void print_record(FILE *out, a2_data *rec){
    fprintf(out, "-->");
    for(int i = 0; i < NUM_FIELDS; i++){
        fprintf(out, " %s: %s ||", field_names[i], rec->fields[i]);
    }
    fprintf(out, "\n");
}

/* 
 * Write all records in the linked list to the output file
 * Iterates through the record list and prints each record
 * Returns the total number of records written
 */
static int write_records(FILE *out, record_list_t *head){
    int cnt = 0; 
    for(record_list_t *p = head; p; p = p->next){ 
        print_record(out, (a2_data*)p->rec); 
        cnt++; 
    }
    fflush(out);  // Ensure all output is written immediately
    return cnt;
}

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
    
    /* Create a new Patricia Trie for storing the data */
    ptree_t *tree = pt_create();

    /* Insert all records into the Patricia Trie using EZI_ADD field as key */
    for(int i = 0; i < n; i++){
        a2_data *d = a2_from_csvRecord(raw[i]);
        const char *key = d->fields[KEY_FIELD];  // KEY_FIELD = 1 (EZI_ADD)
        pt_insert(tree, key, (struct data*)d);
    }
    freeCSV(raw, n);  // Free the raw CSV data as it's now stored in the trie

    /* Process queries from standard input until EOF */
    char *query = NULL;
    while((query = getQuery(stdin))){
        /* Reset performance metrics for this query */
        metrics_reset();
        
        /* Search for the query in the Patricia Trie */
        bool exact = false; 
        pt_node_t *m = pt_search_with_mismatch(tree, query, &exact);
        int count = 0;
        
        /* Check if we found an exact match */
        if(m && exact && m->is_terminal){
            /* Exact match found - output the matching records */
            fprintf(out, "%s \n", query);
            count = write_records(out, m->records);
        } else {
            /* No exact match - find the most similar key using edit distance */
            char *best_key = NULL; 
            record_list_t *best = pt_search_similar_under(m, query, &best_key);
            
            if(best){ 
                /* Similar match found - output the closest records */
                fprintf(out, "%s \n", query); 
                count = write_records(out, best); 
                free(best_key); 
            } else { 
                /* No match found at all */
                printf("%s --> %s\n", query, NOTFOUND); 
                free(query); 
                continue; 
            }
        }
        
        /* Output performance statistics */
        unsigned long long b_out = g_metrics.bitCount + 8ULL;    // Bit comparisons
        unsigned long long n_out = g_metrics.nodeCount;          // Node accesses
        unsigned long long s_out = (g_metrics.stringCount == 0 ? 1ULL : g_metrics.stringCount); // String comparisons
        
        printf("%s --> %d records found - comparisons: b%llu n%llu s%llu\n", 
               query, count, b_out, n_out, s_out);
        free(query);
    }

    /* Clean up resources */
    pt_free(tree);   // Free the Patricia Trie and all associated data
    fclose(csv);     // Close input file
    fclose(out);     // Close output file
    return EXIT_SUCCESS;
}
