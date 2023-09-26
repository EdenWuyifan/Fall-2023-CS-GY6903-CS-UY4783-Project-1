#include "common.h"

#include <cassert>
#include <utility>

// `L`: short for msg_length
// `t`: short for key_length
int diff(int c, int p) {
  int d = c - p;
  if (d < 0) {
    d += 27;
  }
  return d;
}

int ctoi(char c) {
  assert((c == ' ') | ('a' < c) | (c < 'z'));
  if (c == ' ') {
    return 0;
  }
  return (c - 'a' + 1);
}

bool sortByVal(const std::pair<std::size_t, std::size_t> &a,
               const std::pair<std::size_t, std::size_t> &b) {
  return (a.second > b.second);
}

char forward(char m, int amount) {
  amount %= 27;
  if (m == ' ') {
    if (amount == 0) {
      return ' ';
    } else {
      return ('a' + amount);
    }
  }

  return (m - 'a' + amount) % 27 + 'a';
}