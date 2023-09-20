#include <map>
#include <string>
#include <vector>

class CryptAnalysis {
 private:
  /// @brief The target ciphertext will be cracked
  std::string cipher;

  /// @brief The candidate plaintexts for test 1
  std::vector<std::string> dict1;

  /// @brief The candidate plaintexts for test 2
  std::vector<std::string> dict2;

  /// @brief Factors of distances between repeated strings, as the result of
  /// Kasiski analysis
  std::vector<std::pair<size_t, size_t>> factors;

  /// @brief Compute the edit distance of two string. Probably useful for
  /// guessing the best plaintext for test 1.
  /// @return The edit distance of two string
  std::size_t edit_distance(const std::string &a, const std::string &b);

 public:
  CryptAnalysis(std::string cipher, std::vector<std::string> dict1,
                std::vector<std::string> dict2);

  ~CryptAnalysis();

  // Run kasiski analysis and store the result internally
  void kasiski_analysis();
  // Guess the best key length and produces the plaintexts
  void crack();

  // Print the analysis result
  void report();
};

char forward(char m, int amount);
char backward(char c, int amount);