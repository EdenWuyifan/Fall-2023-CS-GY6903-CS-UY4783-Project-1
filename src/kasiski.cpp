#include "kasiski.h"

#include <algorithm>
#include <iostream>
#include <map>

#include "common.h"

KasiskiAnalysis::KasiskiAnalysis(std::string ciphertext)
    : ciphertext(ciphertext) {}

KasiskiAnalysis::~KasiskiAnalysis() {}

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

std::set<std::size_t> KasiskiAnalysis::factorize(std::size_t n) {
  std::set<std::size_t> factors;
  factors.insert(1);
  for (size_t i = 2; i < n / 2; i++) {
    if (n % i == 0) {
      factors.insert(i);
      factors.insert(n / i);
    }
  }
  return factors;
}

std::vector<std::size_t> KasiskiAnalysis::run() {
  std::map<std::string, std::vector<std::size_t>> repeated_strings;
  for (std::size_t t = 3; t <= 24; t++) {
    // std::cout << "analyzing for key length t=" << t << std::endl;
    for (std::size_t i = 0; i < ciphertext.size() - t; i++) {
      if (i + t >= ciphertext.size()) {
        break;
      }
      std::string substr = ciphertext.substr(i, t);
      if (repeated_strings.count(substr) == 0) {
        repeated_strings[substr] = find_all_occurrences(ciphertext, substr);
      }
    }
  }

  for (auto r : repeated_strings) {
    if (r.second.size() <= 1) {
      repeated_strings.erase(r.first);
    }
  }

  std::set<size_t> deltas;
  for (auto r : repeated_strings) {
    std::string substr = r.first;
    std::vector<std::size_t> indicies = r.second;

    for (auto i = indicies.begin(); (i + 1) != indicies.end(); i++) {
      size_t a = *i;
      size_t b = *(i + 1);
      deltas.insert(b - a);
    }
  }

  std::map<size_t, size_t> factor_counts;

  for (auto d : deltas) {
    std::set<size_t> factors = factorize(d);
    for (auto f : factors) {
      // skip factors which are too small or too big
      if ((f < 2) || (f > 24)) {
        continue;
      }
      factor_counts[f] += 1;
    }
  }

  if (factor_counts.count(2) > 0) {
    if (factor_counts.count(4) > 0) {
      factor_counts[4] += factor_counts[2] / 2;
      factor_counts.erase(2);
    }
  }

  factors.assign(factor_counts.begin(), factor_counts.end());
  std::sort(factors.begin(), factors.end(), sortByVal);
  // std::cout << "Factors collected...\n";
  // for (auto fc : factors) {
  //   std::cout << fc.first << ':' << fc.second << std::endl;
  // }

  // std::cout << "End of analysis\n";

  std::vector<std::size_t> answer;
  for (auto it = factors.begin(); it != factors.begin() + 3; it++) {
    answer.push_back((*it).first);
  }

  return answer;
}
