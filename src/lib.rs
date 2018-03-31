#![allow(non_snake_case, non_camel_case_types, non_upper_case_globals)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

pub extern fn handle_sigint(_: i32) {
    unsafe { 
        gShouldStop = 1; 
    }
}
