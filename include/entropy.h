#ifndef ENTROPY_H__
#define ENTROPY_H__

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "common.h"

typedef std::vector<int> Encoded;

Encoded encode(const std::string &text);

void print_encoded(const Encoded &encoded, std::size_t to);

typedef std::map<int, int> Counter;

class TrendsComparison {
 private:
  const std::vector<std::vector<float>> trends;

  std::vector<float> diff_measures;

  float avg;
  float std_dev;

 public:
  TrendsComparison(std::vector<std::vector<float>> trends);
  std::optional<size_t> detect_anomaly();

  float get_std_dev() { return std_dev; }
};

class EntropyAnalysis {
 private:
  const std::string ciphertext;
  Encoded cipher_stream;

  const std::vector<std::string> plaintexts;
  std::vector<Encoded> plain_streams;

  const std::size_t search_space;

  std::vector<Encoded> measure_diffs(const Encoded &cipher_stream);

  float compute_entropy(const Counter &counter);

  Counter make_counter(Encoded::iterator begin, Encoded::iterator end);

  std::vector<float> compute_entropy_trend(Encoded::iterator begin,
                                           Encoded::iterator end, int start);

  std::string char_removed_at(const std::string &s, size_t i);

  std::pair<float, std::string> optimize_entropy_for(
      const std::string &plaintext);

  std::shared_ptr<TrendsComparison> entropy_trend_analysis(
      const Encoded &cipher_stream, std::size_t trend_start);

 public:
  EntropyAnalysis(std::string ciphertext, std::vector<std::string> plaintexts,
                  std::size_t search_space);
  std::optional<std::size_t> run();
};

#endif  // ENTROPY_H__