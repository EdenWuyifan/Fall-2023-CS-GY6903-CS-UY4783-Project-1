#include "main.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

// `L`: short for msg_length
// `t`: short for key_length

CryptAnalysis::CryptAnalysis(std::string cipher, std::vector<std::string> dict1,
                             std::vector<std::string> dict2)
    : cipher(cipher), dict1(dict1), dict2(dict2) {}

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
  // std::vector<size_t> deltas;
  std::map<size_t, size_t> deltas;  // delta => count
  for (auto r : repeated_strings) {
    std::string substr = r.first;
    std::vector<std::size_t> indicies = r.second;

    for (auto i = indicies.begin(); (i + 1) != indicies.end(); i++) {
      size_t a = *i;
      size_t b = *(i + 1);
      size_t delta = b - a;
      if (deltas.count(delta) == 0) {
        deltas[delta] = 0;
      }
      deltas[delta] += 1;
    }
  }
  // some delta values will not make sense (because of the random noises)
  // so we choose the most frequent delta

  for (auto d : deltas) {
    std::size_t delta = d.first;
    std::size_t count = d.second;
    std::cout << delta << ':' << count << '\n';
  }
}

void CryptAnalysis::report() {
  std::cout << "reporting...\n";

  for (auto r : repeated_strings) {
    std::vector<std::size_t> occurences = r.second;
    std::cout << r.first << ": ";
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

std::vector<std::string> parse_dict1() {
  std::string line;
  std::ifstream plain1("resources/plaintext1.txt");
  std::vector<std::string> dict1;

  std::getline(plain1, line);
  for (size_t i = 0; i < 5; i++) {
    for (size_t j = 0; j < 3; j++) {
      std::getline(plain1, line);
    }
    std::getline(plain1, line);
    dict1.push_back(line);
  }
  plain1.close();

  return dict1;
}

std::vector<std::string> parse_dict2() {
  std::string line;
  std::ifstream plain2("resources/plaintext2.txt");
  std::vector<std::string> dict2;

  std::getline(plain2, line);
  std::getline(plain2, line);
  while (std::getline(plain2, line)) {
    dict2.push_back(line);
  }

  plain2.close();

  return dict2;
}

int main(int argc, char *argv[]) {
  std::string ciphertext;

  std::cout << "Input ciphertext:\n";
  std::cin >> ciphertext;

  std::vector<std::string> dict1 = parse_dict1();
  std::vector<std::string> dict2 = parse_dict2();

  // TODO: parse dict 2
  auto analysis = new CryptAnalysis(ciphertext, dict1, dict2);
  analysis->kasiski_analysis();
  analysis->report();
  analysis->crack();

  return 0;
}