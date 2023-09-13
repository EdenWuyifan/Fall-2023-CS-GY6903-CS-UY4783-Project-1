#include "main.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

// 1. Guess the key length
void kasiski_analysis(const std::string &ciphertext) {
  std::map<std::size_t, std::vector<std::size_t>> candidates;

  for (std::size_t key_length = 2; key_length <= 24; key_length++) {
    std::string substr = ciphertext.substr(0, key_length);
    std::size_t reoccurence;
    std::size_t search_from = key_length;
    std::vector<std::size_t> matches;

    while ((reoccurence = ciphertext.find(substr, search_from)) !=
           std::string::npos) {
      matches.push_back(reoccurence);
      search_from = reoccurence + key_length;
      reoccurence = ciphertext.find(substr, search_from);
    }

    candidates[key_length] = matches;
  }

  for (auto candid : candidates) {
    std::size_t t = candid.first;
    std::vector<std::size_t> matches = candid.second;
    std::cout << t << std::endl;
    for (auto m : matches) {
      std::cout << m << ' ';
    }
    std::cout << std::endl;
  }
}

// 2. Try a key of a given key length, and guess the plaintext
std::string crack(const std::string &ciphertext, const int key_len) {}

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

char backward(char c, int amount) {}

void print_plain() {
  std::fstream plaintexts("examples/plaintext1");

  std::string s;
  std::getline(plaintexts, s);

  std::cout << s << std::endl;
}

int main(int argc, char *argv[]) {
  std::string ciphertext = argv[1];
  std::cout << ciphertext << std::endl;

  kasiski_analysis(ciphertext);
  return 0;
}