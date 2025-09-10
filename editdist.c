#include <assert.h>
#include <string.h>
#include "editdist.h"

/* Returns min of 3 integers 
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int min3(int a, int b, int c){
    if (a < b) return (a < c) ? a : c;
    return (b < c) ? b : c;
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int editDistance(char *str1, char *str2, int n, int m){
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    int dp[n + 1][m + 1];
    
    for (int i = 0; i <= n; i++){
        for (int j = 0; j <= m; j++){
            if (i == 0) dp[i][j] = j;
            else if (j == 0) dp[i][j] = i;
            else if (str1[i - 1] == str2[j - 1]){
                dp[i][j] = min3(1 + dp[i - 1][j], 1 + dp[i][j - 1], dp[i - 1][j - 1]);
            } else {
                dp[i][j] = 1 + min3(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
            }
        }
    }
    return dp[n][m];
}
