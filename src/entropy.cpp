#include "entropy.h"

#include <cassert>
#include <cmath>
#include <iostream>

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

  // measure the sum of differences squared for each pair of trends
  for (auto i = trends.begin(); i != trends.end(); i++) {
    for (auto j = i + 1; j != trends.end(); j++) {
      std::vector<float> trend_i = *i;
      std::vector<float> trend_j = *j;
      float trend_diff = 0.0f;
      for (size_t k = 0; k < trend_i.size(); k++) {
        trend_diff += (trend_i[k] - trend_j[k]) * (trend_i[k] - trend_j[k]);
      }

      diff_measures.push_back(trend_diff);
      trend_sum += trend_diff;
      trend_sqr_sum += trend_diff * trend_diff;

      // std::cout << (i - trends.begin()) << ' ' << (j - trends.begin()) << ' '
      //           << trend_diff << '\n';
    }
  }

  float trend_avg = trend_sum / (float)diff_measures.size();
  float trend_var =
      trend_sqr_sum / (float)diff_measures.size() - trend_avg * trend_avg;
  float trend_std = sqrtf32(trend_var);

  std::cout << "avg=" << trend_avg << " var=" << sqrtf32(trend_var) << '\n';

  return std::optional<size_t>();
}

void EntropyAnalysis::run(int end) {
  int i = 0;
  std::vector<std::vector<float>> trends;
  for (auto d : this->diffs) {
    std::vector<float> trend =
        compute_entropy_trend(d.begin(), d.begin() + 24, 8);
    trends.push_back(trend);

    // std::cout << (i + 1) << std::endl;
    // std::cout << std::fixed << std::setprecision(3);
    // for (auto ent : trend) {
    //   std::cout << std::setw(5) << ent << ' ';
    // }
    // std::cout << '\n';

    i++;
  }
  auto answer = detect_trend_anomaly(trends);
  if (answer.has_value()) {
    std::size_t anomaly = answer.value();
    std::cout << anomaly << std::endl;
    return;
  }

  std::cout << -1 << std::endl;
  return;
}
