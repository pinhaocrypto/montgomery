#include "../clean/reduce.h"
#include <immintrin.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "consts.h"
#include "params.h"
#include "reduce.h"

// Define test cases
typedef struct {
  int32_t input;
  int16_t expected_output;
} test_case_t;

// Function to test the clean implementation's montgomery_reduce from AVX2 code
bool test_clean_montgomery_reduce() {
  // Test cases with known input-output pairs
  test_case_t test_cases[] = {
      // Test case 1: a = 0
      {0, 0},

      // Test case 2: a = 3329 << 16 (should reduce to 3329)
      {3329 << 16, 3329},

      // Test case 3: a = 1 << 16 (should reduce to 1)
      {1 << 16, 1},

      // Test case 4: a = 2 << 16 (should reduce to 2)
      {2 << 16, 2},

      // Test case 5: a = (3329 - 1) << 16 (should reduce to 3329 - 1)
      {(3329 - 1) << 16, 3329 - 1},

      // Test case 6: a = (3329 + 1) << 16 (should reduce to 3329 + 1)
      {(3329 + 1) << 16, 3329 + 1},

      // Test case 7: a = -1 * (1 << 16) (should reduce to -1)
      {-1 * (1 << 16), -1},

      // Test case 8: a = -2 * (1 << 16) (should reduce to -2)
      {-2 * (1 << 16), -2},

      // Test case 9: Random large value within valid range
      {12345678, -1207},

      // Test case 10: Another random value
      {-9876543, -1799}};

  int num_tests = sizeof(test_cases) / sizeof(test_case_t);
  bool all_passed = true;

  printf(
      "Testing PQCLEAN_MLKEM512_CLEAN_montgomery_reduce from AVX2 code...\n");

  for (int i = 0; i < num_tests; i++) {
    int32_t input = test_cases[i].input;
    int16_t expected = test_cases[i].expected_output;
    int16_t result = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(input);

    bool passed = (result == expected);
    all_passed &= passed;

    printf("Test %d: input = %d, expected = %d, got = %d - %s\n", i + 1, input,
           expected, result, passed ? "PASSED" : "FAILED");
  }

  // Test linearity property
  printf("\nTesting mathematical properties...\n");

  // Test that montgomery_reduce((a << 16) + (b << 16)) = montgomery_reduce(a <<
  // 16) + montgomery_reduce(b << 16)
  bool linearity_passed = true;
  for (int i = 0; i < 5; i++) {
    // Use fixed values
    int16_t a = 10 + i * 20; // 10, 30, 50, 70, 90
    int16_t b = 15 + i * 15; // 15, 30, 45, 60, 75

    int16_t result1 =
        PQCLEAN_MLKEM512_CLEAN_montgomery_reduce((a << 16) + (b << 16));
    int16_t result2 = a + b;

    bool test_passed = (result1 == result2);
    linearity_passed &= test_passed;

    printf(
        "Linearity test %d: a = %d, b = %d, result1 = %d, result2 = %d - %s\n",
        i + 1, a, b, result1, result2, test_passed ? "PASSED" : "FAILED");
  }
  all_passed &= linearity_passed;

  // Test identity property
  bool identity_passed = true;
  for (int a = -10; a <= 10; a++) {
    int16_t result = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(a << 16);
    bool test_passed = (result == a);
    identity_passed &= test_passed;

    if (!test_passed) {
      printf("Identity test for a = %d: expected = %d, got = %d - FAILED\n", a,
             a, result);
    }
  }
  if (identity_passed) {
    printf("All identity tests PASSED\n");
  }
  all_passed &= identity_passed;

  return all_passed;
}

// Function to test the AVX2 implementation's tomont_avx function
bool test_avx2_tomont() {
  printf("Testing PQCLEAN_MLKEM512_AVX2_tomont_avx function...\n");

  // Create an array of 16 int16_t values (to fill a 256-bit AVX2 vector)
  ALIGNED_INT16(16) test_values;
  ALIGNED_INT16(16) results;

  // Initialize test values (similar to the clean implementation test)
  test_values[0] = 0; // Test case 1: a = 0
  test_values[1] =
      3329 << 16 >> 16;  // Test case 2: a = KYBER_Q (truncated to 16 bits)
  test_values[2] = 1;    // Test case 3: a = 1
  test_values[3] = 2;    // Test case 4: a = 2
  test_values[4] = 3328; // Test case 5: a = KYBER_Q - 1
  test_values[5] = 3330; // Test case 6: a = KYBER_Q + 1
  test_values[6] = -1;   // Test case 7: a = -1
  test_values[7] = -2;   // Test case 8: a = -2

  // More test values
  test_values[8] = 100;
  test_values[9] = 500;
  test_values[10] = 1000;
  test_values[11] = 2000;
  test_values[12] = 3000;
  test_values[13] = -100;
  test_values[14] = -500;
  test_values[15] = -1000;

  // Expected results after Montgomery reduction
  // These values are what we expect after applying Montgomery reduction
  // Note: For AVX2 implementation, we need to adjust expectations based on how
  // tomont_avx works
  int16_t expected[16] = {
      0, // 0 -> 0
      0, // 3329 -> 0 (3329 * R mod q = 0, where R is the Montgomery constant)
      1, // 1 -> 1 (in Montgomery domain)
      2, // 2 -> 2 (in Montgomery domain)
      3328, // 3328 -> 3328 (in Montgomery domain)
      1,    // 3330 -> 1 (3330 mod 3329 = 1)
      -1,   // -1 -> -1 (in Montgomery domain)
      -2,   // -2 -> -2 (in Montgomery domain)
      100,  // 100 -> 100 (in Montgomery domain)
      500,  // 500 -> 500 (in Montgomery domain)
      1000, // 1000 -> 1000 (in Montgomery domain)
      2000, // 2000 -> 2000 (in Montgomery domain)
      3000, // 3000 -> 3000 (in Montgomery domain)
      -100, // -100 -> -100 (in Montgomery domain)
      -500, // -500 -> -500 (in Montgomery domain)
      -1000 // -1000 -> -1000 (in Montgomery domain)
  };

  // Convert test values to AVX2 vectors
  __m256i test_vec[1];
  test_vec[0] = _mm256_loadu_si256((__m256i *)test_values);

  // Apply tomont_avx to the test vector
  PQCLEAN_MLKEM512_AVX2_tomont_avx(test_vec, PQCLEAN_MLKEM512_AVX2_qdata.vec);

  // Store the result back to an array
  _mm256_storeu_si256((__m256i *)results, test_vec[0]);

  // Check results
  bool all_passed = true;
  for (int i = 0; i < 16; i++) {
    bool passed = (results[i] == expected[i]);
    all_passed &= passed;

    printf("Test %d: input = %d, expected = %d, got = %d - %s\n", i + 1,
           test_values[i], expected[i], results[i],
           passed ? "PASSED" : "FAILED");
  }

  return all_passed;
}

int main() {
  // Run the tests
  bool all_tests_passed_clean = test_clean_montgomery_reduce();
  bool all_tests_passed_avx2 = test_avx2_tomont();

  if (all_tests_passed_clean && all_tests_passed_avx2) {
    printf("\nAll tests PASSED!\n");
    return 0;
  }
  printf("\nSome tests FAILED!\n");
  return 1;
}