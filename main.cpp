#include "main.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

// `L`: short for msg_length
// `t`: short for key_length

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
  std::vector<float> trend_diffs;
  trend_diffs.reserve(trends.size() * (trends.size() - 1));
  float trend_sum = 0.0f;
  float trend_sqr_sum = 0.0f;
  for (auto i = trends.begin(); i != trends.end(); i++) {
    for (auto j = i + 1; j != trends.end(); j++) {
      std::vector<float> trend_i = *i;
      std::vector<float> trend_j = *j;
      float trend_diff = 0.0f;
      for (size_t k = 0; k < trend_i.size(); k++) {
        trend_diff += (trend_i[k] - trend_j[k]) * (trend_i[k] - trend_j[k]);
      }

      trend_diffs.push_back(trend_diff);
      trend_sum += trend_diff;
      trend_sqr_sum += trend_diff * trend_diff;

      // std::cout << (i - trends.begin()) << ' ' << (j - trends.begin()) << ' '
      //           << trend_diff << '\n';
    }
  }

  float trend_avg = trend_sum / (float)trend_diffs.size();
  float trend_var =
      trend_sqr_sum / (float)trend_diffs.size() - trend_avg * trend_avg;

  std::cout << "trend_diff_avg=" << trend_avg
            << " trend_diff_var=" << sqrtf32(trend_var) << '\n';

  return std::optional<size_t>();
}

void EntropyAnalysis::run(int end) {
  int i = 0;
  std::vector<std::vector<float>> trends;
  for (auto d : this->diffs) {
    std::vector<float> trend =
        compute_entropy_trend(d.begin(), d.begin() + 16, 8);
    trends.push_back(trend);

    // std::cout << (i + 1) << std::endl;
    // std::cout << std::fixed << std::setprecision(3);
    // for (auto ent : trend) {
    //   std::cout << std::setw(5) << ent << ' ';
    // }
    // std::cout << '\n';

    i++;
  }
  auto x = detect_trend_anomaly(trends);
}

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

std::set<std::size_t> factorize(std::size_t n) {
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

void KasiskiAnalysis::run() {
  std::map<std::string, std::vector<std::size_t>> repeated_strings;
  for (std::size_t t = 3; t <= 24; t++) {
    // std::cout << "analyzing for key length t=" << t << std::endl;
    for (std::size_t i = 0; i < ciphertext.size() - t; i++) {
      if (i + t >= ciphertext.size()) {
        break;
      }
      std::string substr = ciphertext.substr(i, t);
      if (repeated_strings.count(substr)) {
        // already analyzed this substr
        break;
      }
      std::vector<std::size_t> occurences =
          find_all_occurrences(ciphertext, substr);

      repeated_strings[substr] = occurences;
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

  std::cout << "deltas done!\n";

  std::map<size_t, size_t> factor_counts;

  for (auto d : deltas) {
    std::set<size_t> factors = factorize(d);
    for (auto f : factors) {
      // skip factors which are too small or too big
      if ((f < 2) || (f > 24)) {
        continue;
      }
      if (factor_counts.count(f) == 0) {
        factor_counts[f] = 0;
      }
      factor_counts[f] += 1;
    }
  }

  factors.assign(factor_counts.begin(), factor_counts.end());
  std::sort(factors.begin(), factors.end(), sortByVal);
  std::cout << "Factors collected...\n";
  for (auto fc : factors) {
    std::cout << fc.first << ':' << fc.second << std::endl;
  }

  std::cout << "End of analysis\n";
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

std::vector<int> compute_diff(const std::vector<int> &pcode,
                              const std::vector<int> &ccode) {
  std::vector<int> diff(ccode);

  for (size_t i = 0; i < pcode.size(); i++) {
    diff[i] -= pcode[i];
  }
  for (size_t i = pcode.size(); i < ccode.size(); i++) {
    diff.pop_back();
  }

  return diff;
}

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

  auto entropy_analysis = new EntropyAnalysis(ciphertext, plaintexts);
  entropy_analysis->run(40);
  delete entropy_analysis;

  return 0;
}