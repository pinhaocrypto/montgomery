pub const KYBER_Q: i32 = 3329;
pub const Q_INV: i32 = -3327;
pub const R: i32 = 1 << 16;

#[cfg_attr(target_os = "linux", unsafe(no_mangle))]
pub extern "C" fn montgomery_reduce(a: i32) -> i16 {
    let a_mod_r = (a as i16) as i32; // truncate to 16-bit == mod R
    // Key insight: wrapping_mul ensures multiplication result is within i32 range
    // because i16 has signed values, multiplication result may exceed i16 range

    let t = a_mod_r.wrapping_mul(Q_INV);
    let reduced = (a.wrapping_sub(t.wrapping_mul(KYBER_Q))) >> 16;
    reduced as i16
}

#[cfg_attr(target_os = "linux", unsafe(no_mangle))]
fn main() {
    println!("{}", montgomery_reduce(1));
}
