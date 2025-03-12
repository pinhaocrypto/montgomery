use rand::Rng;

/// Kyber's prime modulus
/// In Yang's reduction slide is M
const Q: i32 = 3329;

/// Montgomery parameter R: A power of 2 greater than Q
/// We choose 2^16 = 65536, which is > 3329
const R: i32 = 65536; // 2^16

/// R^2 mod Q: Used for converting to Montgomery form
/// (65536 * 65536) mod 3329 = 1353
const R_SQUARED: i32 = 1353;

/// Multiplicative inverse of -Q modulo R
/// This is the value 'qinv' such that (qinv * Q) ≡ -1 (mod R)
/// For Q = 3329 and R = 2^16, qinv = -3327 
/// In Yang's reduction slide is M'
const Q_INV: i32 = 3327;

/// -------------------------------------------------------------------
/// MONTGOMERY REDUCTION
/// -------------------------------------------------------------------
/// The Montgomery reduction algorithm efficiently computes (A * R^-1) mod Q
/// where:
/// - A is an integer
/// - R is a power of 2 (for efficient division) greater than Q
/// - R and Q are coprime (gcd(R,Q) = 1)
///
/// The key insight is that division by R is expensive, but modulo R is cheap
/// when R is a power of 2 (just a bitmask operation).
///
/// The algorithm works as follows:
/// 1. Compute m = (A mod R) * Q_INV mod R
/// 2. Compute t = (A + m * Q) / R
/// 3. If t ≥ Q, return t - Q, otherwise return t
///
/// This gives the result (A * R^-1) mod Q
/// -------------------------------------------------------------------
pub fn montgomery_reduction(a: i32) -> i32 {
    // Step 1: Calculate m = (a mod R) * Q_INV mod R
    // Since R = 2^16, (a mod R) is just the lower 16 bits of a
    // Example
    // R - 1 = 0xFFFF =                     1111 1111 1111 1111
    // a = 0xABCDEF12 = 1010 1011 1100 1101 1110 1111 0001 0010
    // a & (R-1) = 1110 1111 0001 0010 = 0xEF12
    println!("Q * Q_INV mod R = {}", (Q * Q_INV) & (R - 1));  // Should be R-1 (65535)

    let l = ((a & (R - 1)) * Q_INV) & (R - 1); // AM' mod R
    println!("l = {}", l);
    let result = (a + l * Q) >> 16;
    println!("result = {}", result);

    // Step 3: Final reduction to ensure result is in [0, Q-1]
    if result >= Q { result - Q } else { result }
}

/// -------------------------------------------------------------------
/// CONVERSION TO MONTGOMERY FORM
/// -------------------------------------------------------------------
/// In the Montgomery domain, a number a is represented as (a * R) mod Q
/// To convert a regular number to Montgomery form, we need to compute:
/// a_mont = (a * R) mod Q
///
/// Direct computation requires an expensive modulo operation.
/// A more efficient approach:
/// 1. Compute (a * R^2) mod Q
/// 2. Apply Montgomery reduction to get (a * R^2 * R^-1) mod Q = (a * R) mod Q
/// -------------------------------------------------------------------
pub fn to_montgomery_form(a: i32) -> i32 {
    // Multiply by R^2 mod Q, then apply Montgomery reduction
    montgomery_reduction(a)    
}

/// -------------------------------------------------------------------
/// CONVERSION FROM MONTGOMERY FORM
/// -------------------------------------------------------------------
/// To convert from Montgomery form back to regular form:
/// If a_mont = (a * R) mod Q, we need to compute a = (a_mont * R^-1) mod Q
/// Which is exactly what Montgomery reduction does
/// -------------------------------------------------------------------
pub fn from_montgomery_form(a_mont: i32) -> i32 {
    // Apply Montgomery reduction to get (a_mont * R^-1) mod Q
    montgomery_reduction(a_mont)
}

// pub fn montgomery_multiplication(a: i32, b: i32) -> i32 {



#[cfg(test)]
mod tests {
    use super::*;

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
        use rand::Rng;
        let mut rng = rand::thread_rng();
        for _ in 0..1000 {
            let a: i32 = rng.gen_range(0..(Q * R));
            assert!(check_invariant(a), "Invariant failed for a = {}", a);
        }
    }
}
