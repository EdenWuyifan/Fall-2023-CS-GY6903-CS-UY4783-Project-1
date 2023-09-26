#include "entropy.h"

#include <algorithm>
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

std::vector<Encoded> EntropyAnalysis::measure_diffs(
    const Encoded &cipher_stream) {
  std::vector<Encoded> diffs;
  for (auto &&ps : this->plain_streams) {
    std::vector<int> shift;
    for (size_t i = 0; i < ps.size(); i++) {
      int d = diff(cipher_stream[i], ps[i]);
      shift.push_back(d);
    }
    diffs.push_back(shift);
  }

  return diffs;
}

EntropyAnalysis::EntropyAnalysis(std::string ciphertext,
                                 std::vector<std::string> plaintexts) {
  assert(plaintexts.size() == 5);
  this->ciphertext = ciphertext;
  this->plaintexts = plaintexts;

  this->cipher_stream = encode(this->ciphertext);

  for (const auto &p : this->plaintexts) {
    std::vector<int> plain_stream = encode(p);
    this->plain_streams.push_back(plain_stream);
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

  if (trend_std < 2.5f) {
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

/// @brief Find a new ciphertext, which has a minimum entropy, with a character
/// removed
/// @param ciphertext
/// @return a new ciphertext
std::string EntropyAnalysis::reduce_entropy(const std::string &ciphertext,
                                            std::size_t search_space) {
  std::vector<float> min_ents;
  min_ents.reserve(search_space);
  for (std::size_t ci = 0; ci < search_space * 2; ci++) {
    std::string new_ciphertext = char_removed_at(ciphertext, ci);
    Encoded new_cipher_stream = encode(new_ciphertext);
    std::vector<Encoded> new_diffs = measure_diffs(new_cipher_stream);

    float ent_local_min = 1000.0f;
    for (auto d : new_diffs) {
      Counter cntr = make_counter(d.begin(), d.begin() + search_space);
      float ent = compute_entropy(cntr);
      if (ent < ent_local_min) {
        ent_local_min = ent;
      }
    }
    min_ents.push_back(ent_local_min);
  }

  auto min_i = std::min_element(min_ents.begin(), min_ents.end());
  auto i = min_i - min_ents.begin();

  return char_removed_at(ciphertext, i);
}

std::optional<size_t> EntropyAnalysis::entropy_trend_analysis(
    const Encoded &cipher_stream, std::size_t trend_start) {
  std::vector<std::vector<float>> trends;
  std::vector<Encoded> diffs = measure_diffs(cipher_stream);
  for (auto d : diffs) {
    std::vector<float> trend = compute_entropy_trend(
        d.begin(), d.begin() + trend_start * 3, trend_start);
    trends.push_back(trend);
  }
  return detect_trend_anomaly(trends);
}

void EntropyAnalysis::run(int start) {
  std::vector<Encoded> diffs = measure_diffs(this->cipher_stream);
  auto answer = entropy_trend_analysis(this->cipher_stream, start);
  if (answer.has_value()) {
    std::size_t anomaly = answer.value();
    std::cout << "The ciphertext is encrypted from plaintext " << (anomaly + 1)
              << std::endl;
    return;
  }

  std::string new_ciphertext = reduce_entropy(this->ciphertext, start * 2);
  answer = entropy_trend_analysis(encode(new_ciphertext), start);
  if (answer.has_value()) {
    std::size_t anomaly = answer.value();
    std::cout << "The ciphertext is encrypted from plaintext " << (anomaly + 1)
              << std::endl;
    return;
  }

  new_ciphertext = reduce_entropy(new_ciphertext, start * 2);
  answer = entropy_trend_analysis(encode(new_ciphertext), start);
  if (answer.has_value()) {
    std::size_t anomaly = answer.value();
    std::cout << "The ciphertext is encrypted from plaintext " << (anomaly + 1)
              << std::endl;
    return;
  }

  std::cout << "Cryptanalysis failed to find the plaintext\n";
  return;
}
