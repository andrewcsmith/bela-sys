extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-lib=bela");
    println!("cargo:rustc-link-lib=pthread_rt");
    println!("cargo:rustc-link-lib=xenomai");
    println!("cargo:rustc-link-lib=native");
    println!("cargo:rustc-link-lib=pthread");
    println!("cargo:rustc-link-lib=rt");
    println!("cargo:rustc-link-lib=c");
    println!("cargo:rustc-link-lib=stdc++");
    println!("cargo:rustc-link-search=all=C:/Users/andrewcsmith/workspaces/bin");
    println!("cargo:rustc-link-search=all=C:/Users/andrewcsmith/workspaces/music/belars/lib");
    let bindings = bindgen::Builder::default()
        .header("ext/Bela.h")
        .clang_arg("-Iinclude")
        .clang_arg("-fdeclspec")
        .generate()
        .expect("Unable to generate bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");
}