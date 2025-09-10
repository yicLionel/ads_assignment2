#ifndef PATRICIA_H
#define PATRICIA_H

#include <stdbool.h>
#include "record_struct.h"
#include "metrics.h"

/* Forward declarations for Patricia Trie structures */
struct pt_node;
struct ptree;
struct record_list;

/* 
 * Linked list structure to store multiple records associated with a key
 * Each node contains a pointer to the actual data record and a next pointer
 */
typedef struct record_list {
    struct data *rec;           // Pointer to the data record
    struct record_list *next;   // Pointer to the next record in the list
} record_list_t;

/* 
 * Patricia Trie node structure
 * Each node represents a compressed path segment in the trie
 */
typedef struct pt_node {
    char *label;                // String label for this node (compressed path)
    struct pt_node **children;  // Dynamic array of child node pointers
    int child_count;            // Number of children this node has
    bool is_terminal;           // True if this node represents the end of a key
    record_list_t *records;     // List of records associated with this key
} pt_node_t;

/* 
 * Patricia Trie structure
 * Contains a single root node that serves as the entry point to the trie
 */
typedef struct ptree {
    pt_node_t *root;            // Root node of the Patricia Trie
} ptree_t;

/* 
 * Core Patricia Trie operations
 */

/* Create a new empty Patricia Trie */
ptree_t *pt_create(void);

/* Free the entire Patricia Trie and all associated memory */
void pt_free(ptree_t *t);

/* Insert a key-record pair into the Patricia Trie */
void pt_insert(ptree_t *t, const char *key, struct data *rec);

/* 
 * Search for a key in the Patricia Trie with mismatch detection
 * Returns the node where mismatch occurs or the exact match node
 * Sets *exact_terminal to true if an exact match is found at a terminal node
 */
pt_node_t* pt_search_with_mismatch(ptree_t *t, const char *key, bool *exact_terminal);

/* 
 * Callback function type for tree traversal
 * Called for each complete key found during traversal
 * Parameters: full_key (complete key string), records (associated records), ud (user data)
 */
typedef void (*pt_visit_cb)(const char *full_key, record_list_t *records, void *ud);

/* 
 * Traverse all keys in the subtree rooted at the given node
 * Calls the callback function for each terminal node found
 * Prefix is prepended to each key found during traversal
 */
void pt_traverse_keys_from(pt_node_t *node, const char *prefix, pt_visit_cb cb, void *ud);

/* 
 * Find the most similar key in the subtree using edit distance
 * Searches all keys under mismatch_node and returns the record list
 * of the key with minimum edit distance to the query string
 * If best_key_out is not NULL, stores a copy of the best matching key
 * In case of tie in edit distance, returns lexicographically smallest key
 */
record_list_t* pt_search_similar_under(pt_node_t *mismatch_node,
                                       const char *query,
                                       char **best_key_out);

#endif
