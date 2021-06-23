# bela-sys

C API bindings for the Bela musical interface microprocessor.

## Setup and Linking

This project requires numerous includes and libraries from Bela.
Therefore, to cross-compile you must extract the root partition of [a Bela image](https://github.com/BelaPlatform/bela-image-builder/releases/) to a location of your choice and set the environment variable `BELA_SYSROOT` to point to that directory.

## Testing

I'd love to hear from anyone who has the time and expertise to test this out.
Please check out the doc comment in [`examples/hello.rs`](/andrewcsmith/bela-sys/tree/master/examples/hello.rs) and see if that works for you.

This assumes you've built `libbela.so` and that it's saved on your Bela in the directory `/root/Bela/lib`.

```bash
cargo build --release --target=armv7-unknown-linux-gnueabihf --examples
scp target/armv7-unknown-linux-gnueabihf/release/examples/hello root@bela.local:/root/hello
ssh root@bela.local "LD_LIBRARY_PATH=/root/Bela/lib /root/hello"
```

## Disclaimer

This is nowhere near ready.
Just a little bit of bindgen and some proof-of-concept stuff.
There are problems.
