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

Encoded diff_encoded(const Encoded &cipher, const Encoded &plain) {
  assert(cipher.size() >= plain.size());
  Encoded shift;
  shift.reserve(plain.size());
  for (std::size_t i = 0; i < plain.size(); i++) {
    shift.push_back(diff(cipher[i], plain[i]));
  }
  return shift;
}

void print_encoded(const Encoded &encoded, std::size_t to) {
  for (std::size_t i = 0; i < to; i++) {
    std::cerr << std::setw(2) << std::setfill('0') << encoded[i] << " ";
  }
  std::cerr << '\n';
}

// Measure the difference from the given cipherstream and plainstreams
std::vector<Encoded> EntropyAnalysis::measure_diffs(
    const Encoded &cipher_stream) {
  std::vector<Encoded> diffs;
  for (auto &&ps : this->plain_streams) {
    std::vector<int> shift;
    assert(ps.size() <= cipher_stream.size());
    for (size_t i = 0; i < ps.size(); i++) {
      int d = diff(cipher_stream[i], ps[i]);
      shift.push_back(d);
    }
    diffs.push_back(shift);
  }

  return diffs;
}

EntropyAnalysis::EntropyAnalysis(std::string ciphertext,
                                 std::vector<std::string> plaintexts,
                                 std::size_t search_space)
    : ciphertext(ciphertext),
      plaintexts(plaintexts),
      search_space(search_space) {
  std::cerr << "Entropy Analysis\n";
  assert(plaintexts.size() == 5);

  this->cipher_stream = encode(this->ciphertext);

  for (std::size_t it = 0; it != search_space; it++) {
    // print encoding number with width 2
    std::cerr << std::setw(2) << std::setfill('0') << it << " ";
  }
  std::cerr << '\n';

  print_encoded(cipher_stream, search_space);
  for (const auto &p : this->plaintexts) {
    std::vector<int> plain_stream = encode(p);
    this->plain_streams.push_back(plain_stream);
    print_encoded(plain_stream, search_space);
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

TrendsComparison::TrendsComparison(std::vector<std::vector<float>> trends)
    : trends(trends) {
  diff_measures.reserve(trends.size() * (trends.size() - 1));
  float trend_sum = 0.0f;
  float trend_sqr_sum = 0.0f;

  auto comb_iterator = std::make_unique<Combination>(trends.size(), 2);
  auto combination = comb_iterator->next();

  std::cerr << "[*] Measuring trend difference...\n";
  while (combination.has_value()) {
    std::vector<std::size_t> indicies = combination.value();
    assert(indicies.size() == 2);
    std::size_t i = indicies[0];
    std::size_t j = indicies[1];
    std::vector<float> trend_i = trends[i];
    std::vector<float> trend_j = trends[j];
    float trend_diff = 0.0f;
    for (size_t k = 0; k < trend_i.size(); k++) {
      trend_diff += (trend_i[k] - trend_j[k]) * (trend_i[k] - trend_j[k]);
    }
    std::cerr << "- " << i << " " << j << " " << trend_diff << std::endl;
    diff_measures.push_back(trend_diff);
    trend_sum += trend_diff;
    trend_sqr_sum += trend_diff * trend_diff;

    combination = comb_iterator->next();
  }

  float trend_avg = trend_sum / (float)diff_measures.size();
  float trend_var =
      trend_sqr_sum / (float)diff_measures.size() - trend_avg * trend_avg;
  float trend_std = sqrtf32(trend_var);

  this->avg = trend_avg;
  this->std_dev = trend_std;
}

std::optional<size_t> TrendsComparison::detect_anomaly() {
  std::cerr << "[*] Detecting anomaly...\n";
  if (this->std_dev < 2.0f) {
    std::cerr << "[*] standard deviation is too small (" << this->std_dev
              << ")\n";
    return std::nullopt;
  }

  std::vector<std::size_t> anomaly_indices;
  auto comb_iterator = std::make_unique<Combination>(trends.size(), 2);

  assert(this->diff_measures.size() == comb_iterator->size());

  for (auto diff = diff_measures.begin(); diff != diff_measures.end(); diff++) {
    std::vector<std::size_t> indices = comb_iterator->next().value();
    if (*diff > this->avg + 0.25 * this->std_dev) {
      std::cerr << "Anomaly detected: " << indices[0] << " " << indices[1]
                << " " << *diff << std::endl;
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

std::string EntropyAnalysis::char_removed_at(const std::string &s, size_t i) {
  if (i >= s.size()) {
    return s;
  }
  auto result = s.substr(0, i) + s.substr(i + 1);
  return result;
}

std::pair<float, std::string> EntropyAnalysis::optimize_entropy_for(
    const std::string &plaintext) {
  const Encoded plain_stream = encode(plaintext);
  std::size_t num_trials = (double)search_space * 0.05 + 1;
  std::vector<std::size_t> removed_indices;
  std::vector<float> min_ents;
  std::vector<std::string> min_ciphers;
  min_ents.reserve(num_trials);

  Encoded orig_diff = diff_encoded(this->cipher_stream, plain_stream);
  float start_ent = compute_entropy(
      make_counter(orig_diff.begin(), orig_diff.begin() + search_space));

  std::cerr << "start_ent=" << start_ent << "\n";

  // First, try to remove a single character from the ciphertext
  float prev_ent = start_ent;
  std::size_t min_ci = 0;
  for (std::size_t ci = 0; ci < search_space / 2; ci++) {
    std::string trial_ciphertext = char_removed_at(this->ciphertext, ci);
    Encoded new_cipher_stream = encode(trial_ciphertext);
    Encoded new_diff = diff_encoded(new_cipher_stream, plain_stream);
    Counter cntr =
        make_counter(new_diff.begin(), new_diff.begin() + search_space / 2);
    float ent = compute_entropy(cntr);

    if (ent > prev_ent) {
      // min_ent = ent;
      min_ci = ci - 1;
      break;
    }

    std::cerr << ci << ' ' << std::setprecision(10) << ent << '\n';
    prev_ent = ent;
  }

  std::string reduced_ciphertext = char_removed_at(this->ciphertext, min_ci);

  std::cerr << "min_ent=" << prev_ent << " min_ci=" << min_ci << "\n";

  return std::make_pair(min_ci, reduced_ciphertext);
}

std::shared_ptr<TrendsComparison> EntropyAnalysis::entropy_trend_analysis(
    const Encoded &cipher_stream, std::size_t trend_start) {
  std::vector<std::vector<float>> trends;
  std::vector<Encoded> diffs = measure_diffs(cipher_stream);
  for (auto d : diffs) {
    std::vector<float> trend = compute_entropy_trend(
        d.begin(), d.begin() + trend_start * 3, trend_start);
    trends.push_back(trend);
  }

  auto trends_comparison = std::make_shared<TrendsComparison>(trends);
  return trends_comparison;
}

std::optional<std::size_t> EntropyAnalysis::run() {
  // Analyze the entropy difference on the first `search_space` character diffs
  auto tc = entropy_trend_analysis(this->cipher_stream, search_space);
  auto answer = tc->detect_anomaly();
  if (answer.has_value()) {
    return answer;
  }

  // If the entropy difference is not significant, try to reduce the entropy
  // by removing a single character.
  std::vector<std::pair<float, std::string>> opt_results;
  for (std::size_t pi = 0; pi < 5; pi++) {
    std::cerr << "------------------------\n";
    std::cerr << (pi + 1) << "-th plaintext\n";
    std::pair<float, std::string> opt = optimize_entropy_for(plaintexts[pi]);
    opt_results.push_back(opt);
  }

  std::vector<float> std_devs;
  for (auto it = opt_results.begin(); it != opt_results.end(); it++) {
    std::cerr << (it - opt_results.begin()) << "th optimized ciphertext\n";
    auto tc = entropy_trend_analysis(encode((*it).second), search_space);
    auto anomaly = tc->detect_anomaly();
    if (anomaly.has_value()) {
      return anomaly;
    } else {
      std_devs.push_back(tc->get_std_dev());
    }
  }

  auto max_std = std::max_element(std_devs.begin(), std_devs.end());
  auto max_std_i = max_std - std_devs.begin();

  return std::make_optional(max_std_i);
}
