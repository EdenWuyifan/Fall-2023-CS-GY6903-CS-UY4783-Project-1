#include "main.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>

// `L`: short for msg_length
// `t`: short for key_length

int diff(int c, int p) {
  int d = c - p;
  if (d < 0) {
    d += 27;
  }
  return d;
}

typedef std::map<int, int> Counter;

float entropy(const Counter &counter) {
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

int ctoi(char c) {
  assert(c == ' ' | ('a' < c < 'z'));
  if (c == ' ') {
    return 0;
  }
  return (c - 'a' + 1);
}

std::vector<int> encode(const std::string &text) {
  std::vector<int> encoded;
  encoded.reserve(text.length());
  for (char c : text) {
    encoded.push_back(ctoi(c));
  }
  return encoded;
}

Counter make_counter(const std::vector<int> &diffs) {
  Counter counter;
  for (auto d : diffs) {
    counter[d]++;
  }

  return counter;
}

float entropy_analysis(const std::string &ciphertext,
                       const std::string &plaintext, int end = 200,
                       int start = 8) {
  std::vector<int> cipher_stream = encode(ciphertext);
  std::vector<int> plain_stream = encode(plaintext);

  std::vector<int> diffs;
  diffs.reserve(end);

  for (size_t i = 0; i < end; i++) {
    int d = diff(cipher_stream[i], plain_stream[i]);
    diffs.push_back(d);
  }

  Counter cntr = make_counter(diffs);
  return entropy(cntr);
}

std::vector<float> entropy_trend(const std::string &ciphertext,
                                 const std::string &plaintext, int end = 200,
                                 int start = 8) {
  std::vector<int> cipher_stream = encode(ciphertext);
  std::vector<int> plain_stream = encode(plaintext);

  std::vector<int> diffs;
  diffs.reserve(end);

  for (size_t i = 0; i < start; i++) {
    int d = diff(cipher_stream[i], plain_stream[i]);
    diffs.push_back(d);
  }
  Counter cntr = make_counter(diffs);

  std::vector<float> ent_trend;
  ent_trend.push_back(entropy(cntr));
  for (size_t i = start; i < end; i++) {
    int d = diff(cipher_stream[i], plain_stream[i]);
    cntr[d]++;
    ent_trend.push_back(entropy(cntr));
  }

  return ent_trend;
}

bool sortByVal(const std::pair<size_t, size_t> &a,
               const std::pair<size_t, size_t> &b) {
  return (a.second > b.second);
}

CryptAnalysis::CryptAnalysis(std::string cipher, std::vector<std::string> dict1,
                             std::vector<std::string> dict2)
    : cipher(cipher), dict1(dict1), dict2(dict2) {}

CryptAnalysis::~CryptAnalysis() {}

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

void CryptAnalysis::kasiski_analysis() {
  std::map<std::string, std::vector<std::size_t>> repeated_strings;
  for (std::size_t t = 3; t <= 24; t++) {
    // std::cout << "analyzing for key length t=" << t << std::endl;
    for (std::size_t i = 0; i < cipher.size() - t; i++) {
      if (i + t >= cipher.size()) {
        break;
      }
      std::string substr = cipher.substr(i, t);
      if (repeated_strings.count(substr)) {
        // already analyzed this substr
        break;
      }
      std::vector<std::size_t> occurences =
          find_all_occurrences(cipher, substr);

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

float CryptAnalysis::entropy(const std::unordered_map<int, int> &counter) {
  float ent = .0f;
  for (auto c : counter) {
    float p = (float)(c.second + 1) / 27.;
    ent += p * std::log(p);
  }

  return ent;
}

std::unordered_map<int, int> CryptAnalysis::make_counter(
    const std::vector<int> &diffs) {
  std::unordered_map<int, int> counter;
  for (auto d : diffs) {
    counter[d]++;
  }

  for (auto c : counter) {
    std::cout << c.first << ' ' << c.second << '\n';
  }

  return counter;
}

std::size_t CryptAnalysis::edit_distance(const std::string &a,
                                         const std::string &b) {
  return 0;
}

void CryptAnalysis::crack() {}

void CryptAnalysis::report() {}

char forward(char m, int amount) {
  amount %= 27;
  if (m == ' ') {
    if (amount == 0) {
      return ' ';
    } else {
      return ('a' + amount);
    }
  }

  return (m - 'a' + amount) % 27 + 'a';
}

char backward(char c, int amount) {}

void print_plain() {
  std::fstream plaintexts("examples/plaintext1");

  std::string s;
  std::getline(plaintexts, s);

  std::cout << s << std::endl;
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

void CryptAnalysis::entropy_analysis() {
  auto ccode = encode(this->cipher);
  for (auto pt : this->dict1) {
    auto pcode = encode(pt);
    auto diffs = compute_diff(pcode, ccode);
    auto cntr = make_counter(diffs);
    float ent = entropy(cntr);

    std::cout << ent << '\n';
  }
}

std::vector<float> measure_trend_diff(
    const std::vector<std::vector<float>> &ent_trend_list) {}

int main(int argc, char *argv[]) {
  std::string ciphertext;

  std::cout << "Input ciphertext:\n";
  std::getline(std::cin, ciphertext);

  std::cout << ciphertext << '\n';

  std::vector<std::string> plaintexts_dict = parse_dict1();
  std::vector<std::string> plainwords_dict = parse_dict2();

  std::vector<std::vector<float>> ent_trend_list;
  ent_trend_list.reserve(plaintexts_dict.size());

  for (auto it = plaintexts_dict.begin(); it != plaintexts_dict.end(); it++) {
    std::cout << (it - plaintexts_dict.begin()) + 1 << '\n';
    std::vector<float> ent_trend = entropy_trend(ciphertext, *it, 30, 12);
    ent_trend_list.push_back(ent_trend);
    for (auto ent : ent_trend) {
      std::cout << ent << ' ';
    }
    std::cout << std::endl;
  }

  // auto min_entropy = std::min_element(entropies.begin(), entropies.end());
  // auto min_i = min_entropy - entropies.begin();
  // std::cout << min_i << std::endl;

  // auto analysis = new CryptAnalysis(ciphertext, dict1, dict2);
  // analysis->kasiski_analysis();
  // analysis->report();
  // analysis->entropy_analysis();
  // analysis->crack();

  // delete analysis;

  return 0;
}