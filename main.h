#include <map>
#include <string>
#include <vector>

class CryptAnalysis {
 private:
  /// @brief The target ciphertext will be cracked
  std::string cipher;

  /// @brief The candidate plaintexts for test 1
  std::vector<std::string> plaintexts;

  /// @brief The result of Kasiski analysis
  std::map<std::string, std::vector<std::size_t>> repeated_strings;

  /// @brief Compute the edit distance of two string. Probably useful for
  /// guessing the best plaintext for test 1.
  /// @return The edit distance of two string
  std::size_t edit_distance(const std::string &a, const std::string &b);

 public:
  CryptAnalysis(std::string cipher, std::vector<std::string> plaintexts);
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