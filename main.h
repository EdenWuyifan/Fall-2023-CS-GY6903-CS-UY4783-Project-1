#include <cassert>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

bool sortByVal(const std::pair<size_t, size_t> &a,
               const std::pair<size_t, size_t> &b) {
  return (a.second > b.second);
}

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

int diff(int c, int p) {
  int d = c - p;
  if (d < 0) {
    d += 27;
  }
  return d;
}

int ctoi(char c) {
  assert((c == ' ') | ('a' < c) | (c < 'z'));
  if (c == ' ') {
    return 0;
  }
  return (c - 'a' + 1);
}

typedef std::vector<int> Encoded;

Encoded encode(const std::string &text) {
  Encoded encoded;
  encoded.reserve(text.length());
  for (char c : text) {
    encoded.push_back(ctoi(c));
  }
  return encoded;
}

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

class KasiskiAnalysis {
 private:
  /// @brief The target ciphertext will be cracked
  std::string ciphertext;

  /// @brief Factors of distances between repeated strings, as the result of
  /// Kasiski analysis. (factor => counts) mapping.
  std::vector<std::pair<size_t, size_t>> factors;

 public:
  KasiskiAnalysis(std::string ciphertext);

  ~KasiskiAnalysis();

  // Run kasiski analysis and store the result internally
  void run();
};
