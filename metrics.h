/* 
 * Performance Metrics Header
 * 
 * This header defines structures and functions for tracking performance
 * metrics during Patricia Trie operations. It provides detailed statistics
 * about bit-level comparisons, node accesses, and string comparisons.
 */

#ifndef METRICS_H
#define METRICS_H

/* 
 * Structure to hold performance metrics for Patricia Trie operations
 * Used to measure and compare the efficiency of different algorithms
 */
typedef struct {
    unsigned long long bitCount;    // Total bit comparisons: +8 for each matching character, +1 for first mismatch
    unsigned long long nodeCount;   // Total node accesses: root=1, +1 for each child node examined
    unsigned long long stringCount; // Total string comparisons during similarity matching
} Metrics;

/* Global metrics instance - tracks performance for current operation */
extern Metrics g_metrics;

/* 
 * Reset all performance metrics to zero
 * Should be called before each new search operation
 * Inline function for minimal overhead
 */
static inline void metrics_reset(void){
    g_metrics.bitCount = 0ULL;
    g_metrics.nodeCount = 0ULL;
    g_metrics.stringCount = 0ULL;
}

#endif

