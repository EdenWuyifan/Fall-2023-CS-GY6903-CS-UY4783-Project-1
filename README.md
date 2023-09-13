# General Usage

The cryptanalysis program in written in C++, on the file `main.cpp`.
You need to install `make` to execute the following commands:

- `make run ARGS="ciphertext"` will run cryptanalysis on the given ciphertext
- `make build` will build the cryptanalysis program at `build/main`

# `enc.py`

This is a simple python program you can play around with to get a feel for how encryption works.

## Usage

```
> python enc.py $(cat examples/plaintext1) $(cat examples/key1)
msg=[20, 8, 5, 0, 17, 21, 9, 3, 11, 0, 2, 18, 15, 23, 14, 0, 6, 15, 24, 0, 10, 21, 13, 16, 19, 0, 15, 22, 5, 18, 0, 20, 8, 5, 0, 12, 1, 26, 25, 0, 4, 15, 7]
key=[1, 12, 3, 8, 10]
len(rand_idx)=4 [4, 7, 23, 35]
cipher='ekmjvrfxlkuanuwfoliwgavgxuztlrcoslwbpoaxdghapro'
```