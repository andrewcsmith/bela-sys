extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    let bela_root = PathBuf::from(env::var("BELA_SYSROOT").unwrap_or_else(|_| "/".into()));
    let bela_include = bela_root.join("root/Bela/include");
    let bela_h = bela_include.join("Bela.h");
    if !bela_h.exists() {
        panic!("Set BELA_SYSROOT to the location of your extracted Bela image");
    }
    let bela_lib = bela_root.join("root/Bela/lib");
    let xenomai_lib = bela_root.join("usr/xenomai/lib");
    let local_lib = bela_root.join("usr/local/lib");

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
    println!("cargo:rustc-link-search=all={}", bela_lib.to_str().unwrap());
    println!(
        "cargo:rustc-link-search=all={}",
        xenomai_lib.to_str().unwrap()
    );
    println!(
        "cargo:rustc-link-search=all={}",
        local_lib.to_str().unwrap()
    );
    let bindings = bindgen::Builder::default()
        .header(bela_h.to_str().unwrap())
        .clang_arg(format!("--sysroot={}", bela_root.to_str().unwrap()))
        .clang_arg(format!("-I{}", bela_include.to_str().unwrap()))
        .generate()
        .expect("Unable to generate bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");
}
