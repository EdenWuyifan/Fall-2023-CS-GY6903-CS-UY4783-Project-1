#include "common.h"

#include <cassert>
#include <iostream>
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

Combination::Combination(std::size_t _n, std::size_t _k) {
  n = _n;
  k = _k;

  std::size_t end_ = 1;
  state = std::vector<std::size_t>(k);

  for (std::size_t i = 0; i < k; i++) {
    end_ *= _n;
    end_ /= (i + 1);
    _n -= 1;

    state[i] = i;
  }

  end = end_;
}

// Reference: https://stackoverflow.com/a/39876710
std::optional<std::vector<std::size_t>> Combination::next() {
  if (current == end) return std::nullopt;

  auto comb = std::make_optional(state);

  std::vector<size_t>::iterator last = state.end() - 1;

  while ((last != state.begin()) && (*(last) == n - (state.end() - last))) {
    last--;
  }
  (*last)++;
  while (++last != state.end()) *last = *(last - 1) + 1;

  current += 1;

  return comb;
}
