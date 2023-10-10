# `decryption.py`

The cryptanalysis program in written in Python with `numpy` module.
You need to install `numpy` to run the Python program.
Make sure that you have installed Python versio 3.11.4 or above.

## Installation

First, check your Python version.

```
> python --version
Python 3.11.4
```

Then, install `numpy` module using following command.

```
> python -m pip install numpy
```

## Running the program

The program takes an input ciphertext via stdin.
You can run the program using the following command, with the example ciphertext provided.

```
> python decryption.py < resources/key_4/cipher_1
```

Or, you can run the program with your own ciphertext.

```
> python decryption.py
Input ciphertext: [your ciphertext]
```

After you run, the program outputs what strategy it used to decrypt the ciphertext.
Then, it outputs the 0-based index of the plaintext in the set.
For example, the program outputs `0` if the plaintext is the first element in the set.

```
> python decryption.py < resources/key_4/cipher_1
Input ciphertext:start anomaly
0
```

The result is interpreted as the program decrypted the ciphertext using the start part of the ciphertext
to measure the entropy, and detected anomaly.
For more strategies, see the report.
