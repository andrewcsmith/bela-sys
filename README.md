# bela-sys

C API bindings for the Bela musical interface microprocessor.

## setup and linking

This project assumes you have a number of files in various places:

- `include/` should contain the entire include directories of your Bela system
- `lib/` should include all the libraries you'll be linking to from your Bela
- `ext/` should include the Bela header files themselves

## disclaimer

This is nowhere near ready. Just a little bit of bindgen and some proof-of-concept stuff. There are problems.
