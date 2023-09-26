#ifndef KASISKI_H__
#define KASISKI_H__

#include <set>
#include <string>
#include <vector>

#include "common.h"

class KasiskiAnalysis {
 private:
  /// @brief The target ciphertext will be cracked
  std::string ciphertext;

  /// @brief Factors of distances between repeated strings, as the result of
  /// Kasiski analysis. (factor => counts) mapping.
  std::vector<std::pair<size_t, size_t>> factors;

  std::set<std::size_t> factorize(std::size_t n);

 public:
  KasiskiAnalysis(std::string ciphertext);

  ~KasiskiAnalysis();

  // Run kasiski analysis, and infer a proper key length
  std::vector<std::size_t> run();
};

#endif  // KASISKI_H__