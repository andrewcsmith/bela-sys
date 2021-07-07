#![allow(non_snake_case, non_camel_case_types, non_upper_case_globals)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(feature = "midi")]
pub mod midi {
    include!(concat!(env!("OUT_DIR"), "/midi_bindings.rs"));
}
