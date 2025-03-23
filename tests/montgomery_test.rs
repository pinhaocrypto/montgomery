use montgo_mul::{
    montgomery_reduce,
    fqmul,
    KYBER_Q,
    R
};
pub(crate) use rand::Rng;

// /// Helper function to check the Montgomery reduction invariant.
// /// For any value a, we expect: (montgomery_reduce(a) * R) % Q == a % Q
// fn check_invariant(a: i32) -> bool {
//     let t = montgomery_reduce(a);
//     let lhs = ((t as i32) * R) % Q;
//     let rhs = a % Q;
//     println!("a = {}, t = {}, (t * R) % Q = {}, a % Q = {}", a, t, lhs, rhs);

//     lhs == rhs
// }

// #[test]
// fn test_montgomery_reduction_invariant() {
//     let test_values: Vec<i32> = vec![0, 1, 1234, Q - 1, Q, Q + 1, 12345, 65535, 100000];
//     for a in test_values {
//         println!("Checking invariant for a = {}", a);
//         assert!(check_invariant(a), "Invariant failed for a = {}", a);
//     }
// }

// // #[test]
// // fn test_montgomery_reduction_random() {
// //     let mut rng = rand::thread_rng();
// //     for _ in 0..1000 {
// //         let a: i32 = rng.gen_range(0..(Q * R));
// //         assert!(check_invariant(a), "Invariant failed for a = {}", a);
// //     }
// // }

#[cfg(test)]
mod tests {
    use super::*;
    use rand::Rng;

    const TEST_VALUES: [i32; 8] = [
        0,
        1,
        KYBER_Q - 1,
        KYBER_Q,
        KYBER_Q + 1,
        (R as i32),
        (R as i32) + 1,
        i16::MAX as i32,
    ];

    /// Test montgomery_reduce() range behavior
    #[test]
    fn test_montgomery_reduce_in_range() {
        use rand::Rng;
        let mut rng = rand::thread_rng();
        for _ in 0..10_000 {
            let a: i32 = rng.gen_range(-(KYBER_Q as i32) * (1 << 15)..=(KYBER_Q as i32) * (1 << 15) - 1);
            let r = montgomery_reduce(a);

            assert!(r >= -3329, "Value too small: r = {}", r);
            assert!(r < 3329, "Value too large: r = {}", r);
        }
    }
}