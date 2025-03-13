// Re-export the montgomery module and its contents
pub mod montgomery;

// Explicitly re-export specific items from the montgomery module
// This makes them directly available to external users
pub use crate::montgomery::{
    montgomery_reduction,
    montgomery_multiplication,
    to_montgomery_form,
    from_montgomery_form,
    Q,
    R,
    R_SQUARED,
    Q_INV,
};