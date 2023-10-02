# General Usage

The cryptanalysis program in written in C++.
The project structure is as follows, each directory contains the following:

- `src`: the C++ source code
- `include`: the C++ header files
- `examples`: example ciphertexts and keys
- `build`: the compiled program
- `resources`: the testing data

You need to install `make` to execute the following commands:

- `make`: build the project
- `make all EXPAND=3 KEY_LEN=4`: run cryptanalysis with expand factor 3 on test data encrypted using key length 4. There are test data of key length {4, 6, 8, 12, 17, 24}. The output will be under `results/` directory. You can refer to `results/{KEY_LEN}_{EXPAND}_{CORRECT_ANSWER}.{out, err}` file for the output and error message.

After you run `make all EXPAND=... KEY_LEN=...`, you can visualize the testing result using the following Python script:

```
> python evaluate.py

Expand factor 3
+------------+----------+----------+----------+----------+----------+----------+
| Key Length | Correct1 | Correct2 | Correct3 | Correct4 | Correct5 | Accuracy |
+------------+----------+----------+----------+----------+----------+----------+
|     12     |    X     |    X     |   None   |   None   |   None   |   0.0%   |
|     24     |    X     |    X     |    X     |    X     |   None   |   0.0%   |
|     8      |    X     |    O     |    O     |    O     |    O     |  80.0%   |
|     6      |    O     |    O     |    O     |    O     |    O     |  100.0%  |
|     4      |    O     |    O     |    O     |    O     |    O     |  100.0%  |
|     17     |    X     |    X     |   None   |   None   |   None   |   0.0%   |
+------------+----------+----------+----------+----------+----------+----------+
```

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
