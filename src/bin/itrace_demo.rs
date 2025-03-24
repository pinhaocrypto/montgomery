extern crate montgo_mul;

fn main() {
    println!("{}", montgo_mul::montgomery_reduce(1));
}