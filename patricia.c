/* 
 * Patricia Trie implementation for efficient string matching
 * Supports exact matching and approximate matching with edit distance
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "patricia.h"
#include "a2data.h"
#include "editdist.h"
#include "bit.h"

/* 
 * Create a substring from a bit range of the original string
 * Allocates new memory to hold the numBits specified and fills the allocated
 * memory with the numBits specified starting from the startBit of the oldKey
 * array of bytes.
 * 
 * Parameters:
 *   oldKey - source string to extract from
 *   startBit - starting bit position (0-based)
 *   numBits - number of bits to extract
 * Returns: newly allocated string containing the extracted bits
 */
static char *createStem(const char *oldKey, unsigned int startBit, unsigned int numBits){
    assert(numBits > 0 && startBit >= 0 && oldKey);
    
    // Calculate required bytes for the result
    int extraBytes = 0;
    if((numBits % BITS_PER_BYTE) > 0){
        extraBytes = 1;
    }
    int totalBytes = (numBits / BITS_PER_BYTE) + extraBytes;
    
    // Allocate memory for the new stem
    char *newStem = malloc(sizeof(char) * (totalBytes + 1)); // +1 for null terminator
    assert(newStem);
    
    // Initialize the new stem
    memset(newStem, 0, totalBytes + 1);
    
    // Copy bits from oldKey to newStem
    for(unsigned int i = 0; i < numBits; i++){
        int bit = getBit((char*)oldKey, startBit + i);
        if(bit){
            // Set the corresponding bit in newStem
            unsigned int targetByte = i / BITS_PER_BYTE;
            unsigned int targetBit = BITS_PER_BYTE - (i % BITS_PER_BYTE) - 1;
            newStem[targetByte] |= (1 << targetBit);
        }
    }
    
    return newStem;
}



/* Create a new Patricia trie node */
static pt_node_t* node_new(const char *label){
    pt_node_t *n = calloc(1, sizeof(*n));
    assert(n);
    n->label = strdup(label ? label : "");
    assert(n->label);
    return n;
}

/* Add a record to the end of the record list */
static void records_push(record_list_t **head, struct data *rec){
    record_list_t *r = malloc(sizeof(*r));
    assert(r);
    r->rec = rec;
    r->next = NULL;
    
    // If list is empty, make this the first node
    if(!*head){ 
        *head = r; 
        return; 
    }
    
    // Find the last node and append
    record_list_t *p = *head; 
    while(p->next) p = p->next; 
    p->next = r;
}

/* Recursively free a Patricia trie node and all its children */
static void node_free(pt_node_t *n){
    if(!n) return;
    
    // Recursively free all child nodes
    for(int i = 0; i < n->child_count; i++) 
        node_free(n->children[i]);
    free(n->children);
    
    // Free the record list and the records themselves
    record_list_t *p = n->records; 
    while(p){ 
        record_list_t *next = p->next; 
        a2_free((a2_data*)p->rec);  // Free the a2_data structure
        free(p); 
        p = next; 
    }
    
    // Free node's label and the node itself
    free(n->label);
    free(n);
}

/* Create a new empty Patricia trie */
ptree_t *pt_create(void){
    ptree_t *t = malloc(sizeof(*t));
    assert(t);
    t->root = node_new("");  // Root node with empty label
    return t;
}

/* Free the entire Patricia trie */
void pt_free(ptree_t *t){ 
    if(!t) return; 
    node_free(t->root); 
    free(t); 
}

/* Add a child node to a parent node */
static void add_child(pt_node_t *parent, pt_node_t *child){
    // Expand the children array
    pt_node_t **new_children = realloc(parent->children, sizeof(pt_node_t*) * (parent->child_count + 1));
    assert(new_children);
    parent->children = new_children;
    // Add the new child and increment count
    parent->children[parent->child_count++] = child;
}

/* Find a child node whose label starts with the same character as key */
static int find_candidate_child(pt_node_t *parent, const char *key){
    for(int i = 0; i < parent->child_count; i++){
        if(parent->children[i]->label[0] == key[0]) 
            return i;
    }
    return -1;  // No matching child found
}

/* Calculate longest common prefix with bit-level metrics tracking */
static int lcp_bits(const char *a, const char *b){
    int bit_pos = 0;
    int char_pos = 0;
    
    // Compare bit by bit
    while(a[char_pos] && b[char_pos]){
        // Compare all 8 bits of current character
        for(int bit = 0; bit < BITS_PER_BYTE; bit++){
            int bit_a = getBit((char*)a, bit_pos + bit);
            int bit_b = getBit((char*)b, bit_pos + bit);
            
            if(bit_a != bit_b){
                g_metrics.bitCount += 1ULL;  // Count first mismatch bit
                return char_pos;  // Return character position where mismatch occurs
            }
            g_metrics.bitCount += 1ULL;  // Count each matching bit
        }
        bit_pos += BITS_PER_BYTE;
        char_pos++;
    }
    
    // Handle case where one string is shorter
    if(a[char_pos] || b[char_pos]){
        g_metrics.bitCount += 1ULL;  // Count first mismatch bit
    }
    
    return char_pos;  // Return length of common prefix in characters
}


/* Insert a key-record pair into the Patricia trie */
void pt_insert(ptree_t *t, const char *key, struct data *rec){
    pt_node_t *cur = t->root; 
    const char *rest = key;  // Remaining part of key to insert
    
    while(1){
        int idx = find_candidate_child(cur, rest);
        
        // Case 1: No matching child - create new leaf node
        if(idx < 0){ 
            pt_node_t *leaf = node_new(rest); 
            leaf->is_terminal = true; 
            records_push(&leaf->records, rec); 
            add_child(cur, leaf); 
            return; 
        }
        
        pt_node_t *child = cur->children[idx];
        
        // Calculate longest common prefix using bit-level comparison
        int lcp = 0;
        const char *lab = child->label; 
        int bit_pos = 0;
        
        // Compare bit by bit to find common prefix
        while(rest[lcp] && lab[lcp]){
            bool match = true;
            for(int bit = 0; bit < BITS_PER_BYTE; bit++){
                int bit_rest = getBit((char*)rest, bit_pos + bit);
                int bit_lab = getBit((char*)lab, bit_pos + bit);
                if(bit_rest != bit_lab){
                    match = false;
                    break;
                }
            }
            if(!match) break;
            bit_pos += BITS_PER_BYTE;
            lcp++;
        }
        
        // Case 2: No common prefix - create new sibling
        if(lcp == 0){ 
            pt_node_t *leaf = node_new(rest); 
            leaf->is_terminal = true; 
            records_push(&leaf->records, rec); 
            add_child(cur, leaf); 
            return; 
        }
        
        // Case 3: Partial match - need to split the edge
        if(lcp < (int)strlen(lab)){
            // Create intermediate node with common prefix using createStem for bit-level precision
            char *prefix = createStem(lab, 0, lcp * BITS_PER_BYTE); 
            char *suffix = createStem(lab, lcp * BITS_PER_BYTE, (strlen(lab) - lcp) * BITS_PER_BYTE);
            
            pt_node_t *mid = node_new(prefix); 
            free(prefix);
            
            // Replace child with intermediate node
            cur->children[idx] = mid;
            free(child->label); 
            child->label = suffix; 
            add_child(mid, child);
            
            // Handle remaining part of the key
            const char *remain = rest + lcp;
            if(*remain == '\0'){ 
                // Key ends here - intermediate node becomes terminal
                mid->is_terminal = true; 
                records_push(&mid->records, rec); 
            } else { 
                // Create new leaf for remaining part
                pt_node_t *leaf = node_new(remain); 
                leaf->is_terminal = true; 
                records_push(&leaf->records, rec); 
                add_child(mid, leaf); 
            }
            return;
        } else {
            // Case 4: Complete match - continue down the tree
            rest += lcp; 
            cur = child; 
            if(*rest == '\0'){ 
                // Key ends here - mark current node as terminal
                cur->is_terminal = true; 
                records_push(&cur->records, rec); 
                return; 
            }
        }
    }
}

/* 
 * Search for a key in the Patricia trie, tracking where mismatch occurs
 * Returns the mismatch node and sets exact_terminal if exact match found
 */
pt_node_t* pt_search_with_mismatch(ptree_t *t, const char *key, bool *exact_terminal){
    if(exact_terminal) *exact_terminal = false;
    
    pt_node_t *cur = t->root; 
    const char *rest = key; 
    g_metrics.nodeCount = 0ULL;  // Initialize node count
    
    while(1){
        g_metrics.nodeCount++;  // Count each node visit
        
        int idx = find_candidate_child(cur, rest);
        if(idx < 0) return cur;  // No matching child - mismatch at current node
        
        pt_node_t *child = cur->children[idx];
        
        // Calculate common prefix with bit counting
        int lcp = lcp_bits(rest, child->label);
        
        // Mismatch within the edge label
        if(lcp < (int)strlen(child->label)) return child;
        
        // Continue traversal
        rest += lcp; 
        cur = child;
        
        // Check if we've consumed the entire key
        if(*rest == '\0'){
            if(cur->is_terminal){
                if(exact_terminal) *exact_terminal = true;
            }
            return cur;
        }
    }
}

/* Recursively traverse the trie from a given node, calling callback for each terminal */
static void traverse(pt_node_t *node, const char *prefix, pt_visit_cb cb, void *ud){
    if(!node) return;
    
    // Count node access - each time we look at a new node
    g_metrics.nodeCount++;
    
    // Build the complete key by concatenating prefix and node label
    size_t L1 = strlen(prefix), L2 = strlen(node->label); 
    char *cur = (char*)malloc(L1 + L2 + 1);
    memcpy(cur, prefix, L1); 
    memcpy(cur + L1, node->label, L2); 
    cur[L1 + L2] = '\0';
    
    // If this is a terminal node, call the callback
    if(node->is_terminal) cb(cur, node->records, ud);
    
    // Recursively traverse all children
    for(int i = 0; i < node->child_count; i++) 
        traverse(node->children[i], cur, cb, ud);
    
    free(cur);
}

/* Public interface for tree traversal */
void pt_traverse_keys_from(pt_node_t *node, const char *prefix, pt_visit_cb cb, void *ud){ 
    traverse(node, prefix, cb, ud); 
}

/* Data structure for tracking the best match during similarity search */
typedef struct { 
    const char *query; 
    int best_dist; 
    char *best_key; 
    record_list_t *best_records; 
} sim_ud_t;

/* Callback function to find the best matching key based on edit distance */
static void acc_best(const char *full_key, record_list_t *records, void *ud_){
    sim_ud_t *ud = (sim_ud_t*)ud_;
    g_metrics.stringCount++;  // Count each string comparison
    
    // Calculate edit distance between query and current key
    int d = editDistance((char*)ud->query, (char*)full_key, 
                        (int)strlen(ud->query), (int)strlen(full_key));
    
    // Update best match if this is better (lower distance, or same distance but lexicographically earlier)
    if(!ud->best_key || d < ud->best_dist || 
       (d == ud->best_dist && strcmp(full_key, ud->best_key) < 0)){
        free(ud->best_key); 
        ud->best_key = strdup(full_key); 
        ud->best_dist = d; 
        ud->best_records = records;
    }
}

/* 
 * Find the most similar key in the subtree rooted at mismatch_node
 * Uses edit distance to determine similarity
 * Returns the record list of the best match, optionally outputs the best key
 */
record_list_t* pt_search_similar_under(pt_node_t *mismatch_node, const char *query, char **best_key_out){
    if(!mismatch_node) return NULL;
    
    // Initialize search state
    sim_ud_t ud = {0}; 
    ud.query = query; 
    ud.best_dist = 0x3f3f3f3f;  // Large initial distance
    ud.best_key = NULL; 
    ud.best_records = NULL;
    
    // Traverse all keys in the subtree to find the best match
    pt_traverse_keys_from(mismatch_node, "", acc_best, &ud);
    
    // Return the best key if requested, otherwise free it
    if(best_key_out) 
        *best_key_out = ud.best_key; 
    else 
        free(ud.best_key);
        
    return ud.best_records;
}
