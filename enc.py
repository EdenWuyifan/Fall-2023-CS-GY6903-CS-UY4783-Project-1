import sys
import string
import random


ctoi = { ' ': 0 }

for c in string.ascii_lowercase:
  ctoi[c] = ord(c) - ord('a') + 1

itoc = { i:c for c,i in ctoi.items() }



def construct_key(key: str) -> list[int]:
  """
  "1 12 3" -> [1, 12, 3]
  """
  keys = key.split()
  for k in keys:
    assert 0 <= int(k) <= 26
  return [int(k) for k in keys]


def gen_coin(c_ptr, t, L) -> float:
  return random.random()


def encrypt(msg: list[int], key: list[int]) -> str:
  cipher = []

  i = 0
  num_rand_ch = 0
  
  while len(cipher) < len(msg) + num_rand_ch:
    coin = gen_coin(len(cipher), len(key), len(msg))

    if coin < 0.05:
      j = (len(cipher) + 1) % len(key)
      cipher.append((msg[i] + key[j]) % 27)

  return ''.join(itoc[c] for c in cipher)
  

def main():
  msg = [ctoi[c] for c in sys.argv[1]]
  key = construct_key(sys.argv[2])

  print(f"{msg=}")
  print(f"{key=}")

  cipher = encrypt(msg, key)
  print(f"{cipher=}")


if __name__ == "__main__":
  main()

