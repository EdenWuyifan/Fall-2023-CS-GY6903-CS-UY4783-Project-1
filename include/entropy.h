#ifndef ENTROPY_H__
#define ENTROPY_H__

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "common.h"

typedef std::vector<int> Encoded;

Encoded encode(const std::string &text);

typedef std::map<int, int> Counter;

class EntropyAnalysis {
 private:
  std::string ciphertext;
  Encoded cipher_stream;

  std::vector<std::string> plaintexts;
  std::vector<Encoded> plain_streams;

  std::vector<Encoded> diffs;  // shifts from cipher_stream to each plain_stream

  float compute_entropy(const Counter &counter);

  Counter make_counter(Encoded::iterator begin, Encoded::iterator end);

  std::vector<float> compute_entropy_trend(Encoded::iterator begin,
                                           Encoded::iterator end, int start);

  std::optional<size_t> detect_trend_anomaly(
      std::vector<std::vector<float>> trends);

 public:
  EntropyAnalysis(std::string ciphertext, std::vector<std::string> plaintexts);
  void run(int end);
};

#endif  // ENTROPY_H__