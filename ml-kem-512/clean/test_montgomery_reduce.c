#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Include the necessary headers from the ML-KEM-512 implementation
#include "params.h"
#include "reduce.h"

// Define test cases: input values and expected outputs
typedef struct {
    int32_t input;
    int16_t expected_output;
} test_case_t;

// Function to verify if the montgomery_reduce function works correctly
bool test_montgomery_reduce() {
    // Test cases with known input-output pairs
    test_case_t test_cases[] = {
        // Test case 1: a = 0
        {0, 0},
        
        // Test case 2: a = KYBER_Q << 16 (should reduce to KYBER_Q)
        {KYBER_Q << 16, KYBER_Q},
        
        // Test case 3: a = 1 << 16 (should reduce to 1)
        {1 << 16, 1},
        
        // Test case 4: a = 2 << 16 (should reduce to 2)
        {2 << 16, 2},
        
        // Test case 5: a = (KYBER_Q - 1) << 16 (should reduce to KYBER_Q - 1)
        {(KYBER_Q - 1) << 16, KYBER_Q - 1},
        
        // Test case 6: a = (KYBER_Q + 1) << 16 (should reduce to KYBER_Q + 1)
        {(KYBER_Q + 1) << 16, KYBER_Q + 1},
        
        // Test case 7: a = -1 * (1 << 16) (should reduce to -1)
        {-1 * (1 << 16), -1},
        
        // Test case 8: a = -2 * (1 << 16) (should reduce to -2)
        {-2 * (1 << 16), -2},
        
        // Test case 9: Random large value within valid range
        {12345678, -1207},
        
        // Test case 10: Another random value
        {-9876543, -1799}
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_case_t);
    bool all_passed = true;
    
    printf("Testing PQCLEAN_MLKEM512_CLEAN_montgomery_reduce function...\n");
    
    for (int i = 0; i < num_tests; i++) {
        int32_t input = test_cases[i].input;
        int16_t expected = test_cases[i].expected_output;
        int16_t result = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(input);
        
        bool passed = (result == expected);
        all_passed &= passed;
        
        printf("Test %d: input = %d, expected = %d, got = %d - %s\n", 
               i + 1, input, expected, result, passed ? "PASSED" : "FAILED");
    }
    
    // Additional tests for the mathematical properties
    printf("\nTesting mathematical properties...\n");
    
    // Test that montgomery_reduce((a << 16) + (b << 16)) = montgomery_reduce(a << 16) + montgomery_reduce(b << 16)
    bool linearity_passed = true;
    for (int i = 0; i < 5; i++) {
        // Use fixed values instead of rand() to avoid limited randomness warning
        int16_t a = 10 + i * 20;  // 10, 30, 50, 70, 90
        int16_t b = 15 + i * 15;  // 15, 30, 45, 60, 75
        
        int16_t result1 = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce((a << 16) + (b << 16));
        int16_t result2 = a + b;
        
        bool test_passed = (result1 == result2);
        linearity_passed &= test_passed;
        
        printf("Linearity test %d: a = %d, b = %d, result1 = %d, result2 = %d - %s\n", 
               i + 1, a, b, result1, result2, test_passed ? "PASSED" : "FAILED");
    }
    all_passed &= linearity_passed;
    
    // Test that montgomery_reduce(a * (1 << 16)) = a for small values of a
    bool identity_passed = true;
    for (int a = -10; a <= 10; a++) {
        int16_t result = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(a << 16);
        bool test_passed = (result == a);
        identity_passed &= test_passed;
        
        if (!test_passed) {
            printf("Identity test for a = %d: expected = %d, got = %d - FAILED\n", 
                   a, a, result);
        }
    }
    if (identity_passed) {
        printf("All identity tests PASSED\n");
    }
    all_passed &= identity_passed;
    
    return all_passed;
}

int main() {
    // Run the tests
    bool all_tests_passed = test_montgomery_reduce();
    
    if (all_tests_passed) {
        printf("\nAll tests PASSED!\n");
        return 0;
    } 
    printf("\nSome tests FAILED!\n");
    return 1;
}