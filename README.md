# bela-sys

C API bindings for the Bela musical interface microprocessor.

## setup and linking

This project assumes you have a number of files in various places:

- `include/` should contain the entire include directories of your Bela system
- `lib/` should include all the libraries you'll be linking to from your Bela
- `ext/` should include the Bela header files themselves

## testing

I'd love to hear from anyone who has the time and expertise to test this out.
Please check out the doc comment in
[`src/main.rs`](/andrewcsmith/bela-sys/tree/master/src/main.rs) and see if
that works for you.

This assumes you've built libbela.so and that it's saved on your Bela in
the directory `/root/Bela/lib`.

```bash
cargo build --release --target=armv7-unknown-linux-gnnueabihf --bin hello
scp target/armv7-unknown-linux-gnueabihf/release/hello root@192.168.7.2:/root/hello
ssh root@192.168.7.2 "LD_LIBRARY_PATH=/root/Bela/lib /root/hello"
```

## disclaimer

This is nowhere near ready. Just a little bit of bindgen and some
proof-of-concept stuff. There are problems.
