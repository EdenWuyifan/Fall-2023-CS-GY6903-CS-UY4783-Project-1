import sys
import string
import random

COIN_THRESHOLD = 0.05

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

  cipher_ptr = 0
  msg_ptr = 0
  num_rand_ch = 0
  
  while len(cipher) < len(msg) + num_rand_ch:
    coin = gen_coin(cipher_ptr, len(key), len(msg))

    if coin >= COIN_THRESHOLD:
      j = (msg_ptr + 1) % len(key)
      cipher.append((msg[msg_ptr] + key[j]) % 27)
      msg_ptr += 1
    else:
      c = random.randint(0, 26)
      cipher.append(c)
      num_rand_ch += 1
    cipher_ptr += 1


  return num_rand_ch, ''.join(itoc[c] for c in cipher)
  

def main():
  msg = [ctoi[c] for c in sys.argv[1]]
  key = construct_key(sys.argv[2])

  print(f"{msg=}")
  print(f"{key=}")

  num_rand_ch, cipher = encrypt(msg, key)
  print(f"{num_rand_ch=}")
  print(f"{cipher=}")


if __name__ == "__main__":
  main()

