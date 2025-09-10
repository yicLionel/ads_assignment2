#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit.h"

// 复制 createStem 函数用于测试
static char *createStem(const char *oldKey, unsigned int startBit, unsigned int numBits){
    if(numBits == 0 || !oldKey) return NULL;
    
    // Calculate required bytes for the result
    int extraBytes = 0;
    if((numBits % BITS_PER_BYTE) > 0){
        extraBytes = 1;
    }
    int totalBytes = (numBits / BITS_PER_BYTE) + extraBytes;
    
    // Allocate memory for the new stem
    char *newStem = malloc(sizeof(char) * (totalBytes + 1)); // +1 for null terminator
    if(!newStem) return NULL;
    
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

int main(){
    printf("Testing createStem function:\n\n");
    
    const char *test_string = "Hello";
    printf("Original string: \"%s\"\n", test_string);
    printf("String length: %zu characters\n", strlen(test_string));
    printf("String length: %zu bits\n\n", strlen(test_string) * BITS_PER_BYTE);
    
    // Test 1: Extract first 8 bits (first character)
    printf("Test 1: Extract first 8 bits (first character)\n");
    char *first_char = createStem(test_string, 0, 8);
    if(first_char){
        printf("Result: \"%s\"\n", first_char);
        free(first_char);
    }
    
    // Test 2: Extract second 8 bits (second character)
    printf("\nTest 2: Extract second 8 bits (second character)\n");
    char *second_char = createStem(test_string, 8, 8);
    if(second_char){
        printf("Result: \"%s\"\n", second_char);
        free(second_char);
    }
    
    // Test 3: Extract first 4 bits
    printf("\nTest 3: Extract first 4 bits\n");
    char *first_4_bits = createStem(test_string, 0, 4);
    if(first_4_bits){
        printf("Result: \"%s\"\n", first_4_bits);
        free(first_4_bits);
    }
    
    // Test 4: Extract middle 16 bits (2 characters)
    printf("\nTest 4: Extract middle 16 bits (2 characters)\n");
    char *middle_2_chars = createStem(test_string, 8, 16);
    if(middle_2_chars){
        printf("Result: \"%s\"\n", middle_2_chars);
        free(middle_2_chars);
    }
    
    printf("\ncreateStem function test completed!\n");
    return 0;
}
