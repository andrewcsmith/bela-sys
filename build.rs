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
        .allowlist_type("Bela.*")
        .allowlist_function("Bela_.*")
        .blocklist_function("Bela_userSettings")
        .allowlist_function("rt_.*printf")
        .allowlist_var("BELA_.*")
        .allowlist_var("DEFAULT_.*")
        .generate()
        .expect("Unable to generate bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");

    #[cfg(feature = "midi")]
    {
        let midi_root = bela_root.join("root/Bela/libraries/Midi");

        // TODO: asound is in usr/lib/arm-linux-gnueabihf, so copy it root/Bela/lib,
        // as otherwise the link step will also find libpthread.so and other link
        // scripts there which reference absolute paths. Is there a cleaner
        // solution?
        let asound_lib = bela_root.join("usr/lib/arm-linux-gnueabihf/libasound.so");
        let asound_copy = bela_lib.join("libasound.so");
        if !asound_copy.exists() {
            std::fs::copy(asound_lib, asound_copy).unwrap();
        }

        cc::Build::new()
            .cpp(true)
            .object(midi_root.join("build/Midi_c.o"))
            .object(midi_root.join("build/Midi.o"))
            .compile("midi");
        println!("cargo:rustc-link-lib=asound");
        println!("cargo:rustc-link-lib=modechk");

        let midi_h = midi_root.join("Midi_c.h");
        let bindings = bindgen::Builder::default()
            .header(midi_h.to_str().unwrap())
            .clang_arg(format!("--sysroot={}", bela_root.to_str().unwrap()))
            .clang_arg(format!("-I{}", bela_include.to_str().unwrap()))
            .allowlist_function("Midi_.*")
            .generate()
            .expect("Unable to generate bindings");
        bindings
            .write_to_file(out_path.join("midi_bindings.rs"))
            .expect("Couldn't write bindings");
    }
}
