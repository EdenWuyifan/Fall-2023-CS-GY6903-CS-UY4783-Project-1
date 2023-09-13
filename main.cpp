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
      std::vector<std::size_t> occurences =
          find_all_occurrences(cipher, substr);
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

void CryptAnalysis::crack() {
  // Guess key lengths
  // GCD works if there is no (believed) random characters!

  // compute deltas (key_length => deltas)
  std::map<size_t, std::vector<size_t>> deltas;
  for (auto r : repeated_strings) {
    std::string substr = r.first;
    std::vector<std::size_t> indicies = r.second;

    for (auto i = indicies.begin(); (i + 1) != indicies.end(); i++) {
      size_t a = *i;
      size_t b = *(i + 1);
      size_t delta = b - a;
      deltas[substr.size()].push_back(delta);
    }
  }
  // some delta values will not make sense (because of the random noises)

  for (auto d : deltas) {
    std::size_t key_len = d.first;
    std::vector<std::size_t> ds = d.second;

    std::cout << key_len << ": ";
    for (auto d : ds) {
      std::cout << d << ' ';
    }
    std::cout << '\n';
  }
}

void CryptAnalysis::report() {
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
  analysis->crack();

  return 0;
}