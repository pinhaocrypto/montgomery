use rand::Rng;

/// Kyber's prime modulus
/// In Yang's reduction slide is M
pub const Q: i32 = 3329;

/// Montgomery parameter R: A power of 2 greater than Q
/// We choose 2^16 = 65536, which is > 3329
pub const R: i32 = 65536; // 2^16

/// R^2 mod Q: Used for converting to Montgomery form
/// (65536 * 65536) mod 3329 = 1353
pub const R_SQUARED: i32 = 1353;

/// Multiplicative inverse of -Q modulo R
/// This is the value 'qinv' such that (qinv * Q) ≡ -1 (mod R)
/// For Q = 3329 and R = 2^16, qinv = -3327
/// In Yang's reduction slide is M'
pub const Q_INV: i32 = 3327;

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
    let l = ((a & (R - 1)) * Q_INV) & (R - 1); // AM' mod R
    let result = (a + l * Q) >> 16;
    if result >= Q { result - Q } else { result }
}

pub fn montgomery_multiplication(a: i32, b: i32) -> i32 {
    let b_precomputed = b * R % Q;
    montgomery_reduction(a * b_precomputed)
}
