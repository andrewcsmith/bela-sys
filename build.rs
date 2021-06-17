extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    #[cfg(feature = "static")]
    {
        println!("cargo:rustc-link-lib=static=bela");
        println!("cargo:rustc-link-lib=stdc++");
    }
    #[cfg(not(feature = "static"))]
    {
        println!("cargo:rustc-link-lib=bela");
    }
    println!("cargo:rustc-link-lib=cobalt");
    println!("cargo:rustc-link-lib=prussdrv");
    println!(
        "cargo:rustc-link-search=all={}/lib",
        env::var("CARGO_MANIFEST_DIR").unwrap()
    );
    let bindings = bindgen::Builder::default()
        .header("ext/Bela.h")
        .clang_arg("-Iinclude")
        .clang_arg("-Iext")
        .clang_arg("-fdeclspec")
        .generate()
        .expect("Unable to generate bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");
}
