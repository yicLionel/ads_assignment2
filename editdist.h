/* 
 * Edit distance calculation functions
 * Implements Levenshtein distance algorithm for string similarity
 */
#ifndef EDITDIST_H
#define EDITDIST_H

/* Calculate edit distance between two strings */
int editDistance(char *str1, char *str2, int n, int m);

/* Helper function to find minimum of three integers */
int min3(int a, int b, int c);

#endif
