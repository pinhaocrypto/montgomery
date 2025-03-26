#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/* Functions to print binary representation */
void print_binary16(int16_t x) {
  printf("Binary (16-bit): ");
  for (int i = 15; i >= 0; i--) {
    printf("%d", (x >> i) & 1);
    if (i % 4 == 0 && i > 0)
      printf(" "); // Add space every 4 bits for readability
  }
  printf("\n");
}

void print_binary32(int32_t x) {
  printf("Binary (32-bit): ");
  for (int i = 31; i >= 0; i--) {
    printf("%d", (x >> i) & 1);
    if (i % 4 == 0 && i > 0)
      printf(" "); // Add space every 4 bits for readability
  }
  printf("\n");
}

/* Don't change parameters below this line */

#define KYBER_Q 3329

#define MONT (-1044) // 2^16 mod q
#define QINV (-3327) // q^-1 mod 2^16

/*************************************************
 * Name:        PQCLEAN_MLKEM512_CLEAN_montgomery_reduce
 *
 * Description: Montgomery reduction; given a 32-bit integer a, computes
 *              16-bit integer congruent to a * R^-1 mod q, where R=2^16
 *
 * Arguments:   - int32_t a: input integer to be reduced;
 *                           has to be in {-q2^15,...,q2^15-1}
 *
 * Returns:     integer in {-q+1,...,q-1} congruent to a * R^-1 modulo q.
 **************************************************/
int16_t PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(int32_t a) {
  int16_t t;

  printf("\n--- Montgomery Reduction Steps ---\n");
  printf("Input a = %" PRId32 "\n", a);
  print_binary32(a);

  t = (int16_t)a * QINV;
  printf("\nStep 1: t = (int16_t)a * QINV\n");
  printf("QINV = %" PRId16 "\n", QINV);
  printf("(int16_t)a = %" PRId16 " (truncated to 16 bits)\n", (int16_t)a);
  print_binary16((int16_t)a);
  printf("t = %" PRId16 "\n", t);
  print_binary16(t);

  int32_t t32 = (int32_t)t;
  printf("\nStep 2: Convert t to 32-bit\n");
  printf("t32 = (int32_t)t = %" PRId32 "\n", t32);
  print_binary32(t32);

  int32_t t32_mul_q = t32 * KYBER_Q;
  printf("\nStep 3: t32 * KYBER_Q\n");
  printf("KYBER_Q = %d\n", KYBER_Q);
  printf("t32 * KYBER_Q = %" PRId32 "\n", t32_mul_q);
  print_binary32(t32_mul_q);

  int32_t a_minus_t32q = a - t32_mul_q;
  printf("\nStep 4: a - (t32 * KYBER_Q)\n");
  printf("a - (t32 * KYBER_Q) = %" PRId32 "\n", a_minus_t32q);
  print_binary32(a_minus_t32q);

  t = a_minus_t32q >> 16;
  printf("\nStep 5: Final result after right shift by 16\n");
  printf("t = (a - (t32 * KYBER_Q)) >> 16 = %" PRId16 "\n", t);
  print_binary16(t);
  printf("-----------------------------------\n\n");

  return t;
}

int main() {
  int32_t a = 1000000000;
  printf("Testing Montgomery reduction with a = %" PRId32 "\n", a);
  int16_t reduce_t = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(a);
  printf("Final result: %" PRId16 "\n", reduce_t);

  // Test with a few more values to see the pattern
  printf("\nTesting with a few more values:\n");
  int32_t test_values[] = {361273681, 361273682, 361273683, KYBER_Q << 16};
  for (int i = 0; i < 4; i++) {
    printf("\nTest with a = %" PRId32 "\n", test_values[i]);
    int16_t result = PQCLEAN_MLKEM512_CLEAN_montgomery_reduce(test_values[i]);
    printf("Result = %" PRId16 "\n", result);
  }

  return 0;
}