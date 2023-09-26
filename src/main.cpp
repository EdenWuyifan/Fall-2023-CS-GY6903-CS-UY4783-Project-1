#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

#include "common.h"
#include "entropy.h"
#include "kasiski.h"

static std::vector<std::string> parse_dict1();
static std::vector<std::string> parse_dict2();

int main(int argc, char *argv[]) {
  std::string ciphertext;

  if (argc < 2) {
    std::cerr << "Usage: main <1|2>\n1 for test 1\n2 for test 2\n";
    exit(2);
  }

  std::string test = argv[1];

  if (test == "2") {
    std::cerr << "Not implemented yet\n";
    return 0;
  }

  std::cout << "Input ciphertext:\n";
  std::getline(std::cin, ciphertext);

  std::vector<std::string> plaintexts = parse_dict1();
  std::vector<std::string> plainwords = parse_dict2();

  auto kasiski_analysis = new KasiskiAnalysis(ciphertext);
  auto factors = kasiski_analysis->run();
  delete kasiski_analysis;

  // for (auto f : factors) {
  //   std::cout << f << ' ';
  // }
  // std::cout << std::endl;

  auto entropy_analysis = new EntropyAnalysis(ciphertext, plaintexts);
  entropy_analysis->run(factors[0] * 2);
  delete entropy_analysis;

  return 0;
}

static std::vector<std::string> parse_dict1() {
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

static std::vector<std::string> parse_dict2() {
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
