// Import each item directly from the crate root
// This leverages our explicit re-exports in lib.rs
use montgo_mul::{
    montgomery_reduction,
    montgomery_multiplication,
    Q,
    R
};
pub(crate) use rand::Rng;

/// Helper function to check the Montgomery reduction invariant.
/// For any value a, we expect: (montgomery_reduction(a) * R) % Q == a % Q
fn check_invariant(a: i32) -> bool {
    let t = montgomery_reduction(a);
    println!("t = {}", t);
    println!("(t * R) % Q = {}", (t * R) % Q);
    (t * R) % Q == a % Q
}

#[test]
fn test_montgomery_reduction_invariant() {
    let test_values = vec![0, 1, 1234, Q - 1, Q, Q + 1, 12345, 65535, 100000];
    for a in test_values {
        println!("Checking invariant for a = {}", a);
        assert!(check_invariant(a), "Invariant failed for a = {}", a);
    }
}

#[test]
fn test_montgomery_reduction_random() {
    let mut rng = rand::thread_rng();
    for _ in 0..1000 {
        let a: i32 = rng.gen_range(0..(Q * R));
        assert!(check_invariant(a), "Invariant failed for a = {}", a);
    }
}

#[test]
fn test_montgomery_multiplication() {
    // Test basic multiplication
    let test_pairs = vec![
        (1, 1, 1),                   // 1 * A = A
        (2, 3, 6),                   // Basic multiplication
        (10, 20, 200 % Q),           // Modular reduction for larger products
        (Q - 1, 2, (Q - 1) * 2 % Q), // Near modulus
        (Q, 5, 0),                   // Multiple of modulus
        (0, 42, 0),                  // Zero property
    ];

    for (a, b, expected) in test_pairs {
        let result = montgomery_multiplication(a, b);
        println!("{}*{} = {} (expected {})", a, b, result, expected);
        assert_eq!(result, expected, "Failed for {}*{}", a, b);
    }
}

#[test]
fn test_montgomery_multiplication_random() {
    let mut rng = rand::thread_rng();

    // Test with random values
    for _ in 0..100 {
        let a: i32 = rng.gen_range(0..Q);
        let b: i32 = rng.gen_range(0..Q);

        // Calculate expected result using standard modular multiplication
        let expected = ((a as i64 * b as i64) % Q as i64) as i32;

        let result = montgomery_multiplication(a, b);
        assert_eq!(result, expected, "Failed for random values {}*{}", a, b);
    }
}
