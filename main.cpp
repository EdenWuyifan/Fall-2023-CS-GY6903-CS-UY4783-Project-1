#include "main.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

// `L`: short for msg_length
// `t`: short for key_length

CryptAnalysis::CryptAnalysis(std::string cipher,
                             std::vector<std::string> plaintexts) {
  this->cipher = cipher;
  this->plaintexts = plaintexts;
}

CryptAnalysis::~CryptAnalysis() {}

std::vector<std::size_t> find_all_occurrences(const std::string &s,
                                              const std::string &niddle) {
  std::vector<std::size_t> occurrences;
  std::size_t pos = s.find(niddle);
  while (pos != std::string::npos) {
    occurrences.push_back(pos);
    pos = s.find(niddle, pos + niddle.size());
  }

  return occurrences;
}

void CryptAnalysis::kasiski_analysis() {
  std::map<std::string, std::vector<std::size_t>> repeated_strings;
  for (std::size_t t = 2; t <= 24; t++) {
    // std::cout << "analyzing for key length t=" << t << std::endl;
    for (std::size_t i = 0; i < cipher.size() - t; i++) {
      if (i + t >= cipher.size()) {
        break;
      }
      std::string substr = cipher.substr(i, t);
      if (repeated_strings.count(substr)) {
        // already analyzed this substr
        break;
      }
      std::cout << "trying substr=" << substr;
      std::vector<std::size_t> occurences =
          find_all_occurrences(cipher, substr);
      std::cout << ", found " << occurences.size() << " occurrences\n";
      repeated_strings[substr] = occurences;
    }
  }

  for (auto r : repeated_strings) {
    if (r.second.size() > 1) {
      this->repeated_strings[r.first] = r.second;
    }
  }
}

std::size_t CryptAnalysis::edit_distance(const std::string &a,
                                         const std::string &b) {}

std::string CryptAnalysis::crack() {}

// 1. Guess the key length
void CryptAnalysis::report() {
  // key_length => substring occurence map

  std::cout << "reporting...\n";

  for (auto r : repeated_strings) {
    std::string s = r.first;
    std::vector<std::size_t> occurences = r.second;
    std::cout << s << '\n';
    for (auto m : occurences) {
      std::cout << m << ' ';
    }
    std::cout << std::endl;
  }
}

// 2. Try a key of a given key length, and guess the plaintext
std::string CryptAnalysis::crack(const std::string &ciphertext,
                                 const int key_len) {}

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
  std::cout << "cipher=" << ciphertext << std::endl;

  std::vector<std::string> dictionary;
  auto analysis = new CryptAnalysis(ciphertext, dictionary);
  analysis->kasiski_analysis();
  analysis->report();

  // kasiski_analysis(ciphertext);
  return 0;
}