#include "entropy.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <unordered_map>

Encoded encode(const std::string &text) {
  Encoded encoded;
  encoded.reserve(text.length());
  for (char c : text) {
    encoded.push_back(ctoi(c));
  }
  return encoded;
}

EntropyAnalysis::EntropyAnalysis(std::string ciphertext,
                                 std::vector<std::string> plaintexts) {
  assert(plaintexts.size() == 5);
  this->ciphertext = ciphertext;
  this->plaintexts = plaintexts;

  this->cipher_stream = encode(this->ciphertext);
  for (auto p : this->plaintexts) {
    std::vector<int> plain_stream = encode(p);
    this->plain_streams.push_back(plain_stream);

    std::vector<int> shift;
    for (size_t i = 0; i < plain_stream.size(); i++) {
      int d = diff(cipher_stream[i], plain_stream[i]);
      shift.push_back(d);
    }
    this->diffs.push_back(shift);
  }
}

float EntropyAnalysis::compute_entropy(const Counter &counter) {
  float ent = .0f;
  int all_counts = 0;
  for (auto cnt : counter) {
    all_counts += cnt.second;
  }
  for (auto cnt : counter) {
    float p = (float)cnt.second / (float)all_counts;
    ent -= p * log(p);
  }

  return ent;
}

Counter EntropyAnalysis::make_counter(std::vector<int>::iterator begin,
                                      std::vector<int>::iterator end) {
  Counter counter;
  for (auto it = begin; it != end; it++) {
    counter[*it]++;
  }
  return counter;
}

std::vector<float> EntropyAnalysis::compute_entropy_trend(
    Encoded::iterator diff_begin, Encoded::iterator diff_end, int initial) {
  assert(diff_begin + initial <= diff_end);
  std::vector<float> trend;

  Counter counter = make_counter(diff_begin, diff_begin + initial);
  trend.push_back(compute_entropy(counter));

  for (auto it = diff_begin + initial; it != diff_end - 1; it++) {
    counter[*it]++;
    trend.push_back(compute_entropy(counter));
  }

  return trend;
}

std::optional<size_t> EntropyAnalysis::detect_trend_anomaly(
    std::vector<std::vector<float>> trends) {
  std::vector<float> diff_measures;
  diff_measures.reserve(trends.size() * (trends.size() - 1));
  float trend_sum = 0.0f;
  float trend_sqr_sum = 0.0f;

  auto comb = std::make_unique<Combination>(trends.size(), 2);
  auto it = comb->next();

  // measure the sum of differences squared for each pair of trends
  while (it.has_value()) {
    std::vector<std::size_t> indicies = it.value();
    assert(indicies.size() == 2);
    std::size_t i = indicies[0];
    std::size_t j = indicies[1];
    std::vector<float> trend_i = trends[i];
    std::vector<float> trend_j = trends[j];
    float trend_diff = 0.0f;
    for (size_t k = 0; k < trend_i.size(); k++) {
      trend_diff += (trend_i[k] - trend_j[k]) * (trend_i[k] - trend_j[k]);
    }
    diff_measures.push_back(trend_diff);
    trend_sum += trend_diff;
    trend_sqr_sum += trend_diff * trend_diff;

    it = comb->next();
  }

  float trend_avg = trend_sum / (float)diff_measures.size();
  float trend_var =
      trend_sqr_sum / (float)diff_measures.size() - trend_avg * trend_avg;
  float trend_std = sqrtf32(trend_var);

  std::cout << "avg=" << trend_avg << " std=" << trend_std << '\n';

  if (trend_std < 1.0f) {
    std::cerr << "No general anomaly is detected\n";
    return std::nullopt;
  }

  // collect anomaly indices
  std::vector<std::size_t> anomaly_indices;
  comb = std::make_unique<Combination>(trends.size(), 2);

  assert(diff_measures.size() == comb->size());

  for (auto diff = diff_measures.begin(); diff != diff_measures.end(); diff++) {
    std::vector<std::size_t> indices = comb->next().value();
    if (*diff > trend_avg + 0.25 * trend_std) {
      anomaly_indices.insert(anomaly_indices.end(), indices.begin(),
                             indices.end());
    }
  }

  std::unordered_map<std::size_t, std::size_t>
      ai_count;  // anomaly indices count

  for (auto i : anomaly_indices) {
    ai_count[i]++;
  }

  int most_frequent = -1;
  std::size_t max_count = 0;
  for (const auto &i_cnt : ai_count) {
    if (i_cnt.second > max_count) {
      max_count = i_cnt.second;
      most_frequent = i_cnt.first;
    }
  }

  // Is the most frequent index uninque?
  int mf_count = 0;
  for (const auto &i_cnt : ai_count) {
    if (i_cnt.second == max_count) {
      mf_count++;
    }
  }

  if (mf_count != 1) {
    std::cerr << "Most frequent index is not unique\n";
    return std::nullopt;
  }

  return std::optional<size_t>(most_frequent);
}

void EntropyAnalysis::run(int end) {
  int i = 0;
  std::vector<std::vector<float>> trends;
  for (auto d : this->diffs) {
    std::vector<float> trend =
        compute_entropy_trend(d.begin(), d.begin() + 24, 8);
    trends.push_back(trend);

    i++;
  }
  auto answer = detect_trend_anomaly(trends);
  if (answer.has_value()) {
    std::size_t anomaly = answer.value();
    std::cout << "The ciphertext is encrypted from plaintext " << (anomaly + 1)
              << std::endl;
    return;
  }

  std::cout << "Cryptanalysis failed to find the plaintext\n";
  return;
}
