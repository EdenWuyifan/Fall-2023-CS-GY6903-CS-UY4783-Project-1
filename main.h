#include <map>
#include <string>
#include <vector>

class CryptAnalysis {
 private:
  /// @brief The target ciphertext will be cracked
  std::string cipher;

  /// @brief The candidate plaintexts for test 1
  std::vector<std::string> candidates;

  /// @brief The result of Kasiski analysis, for each key length (2~24)
  std::map<std::size_t, std::vector<size_t>> displacements;

  /// @brief Compute the edit distance of two string. Probably useful for
  /// guessing the best plaintext for test 1.
  /// @return The edit distance of two string
  std::size_t edit_distance(const std::string &a, const std::string &b);

 public:
  CryptAnalysis(std::string cipher, std::vector<std::string> candidates);
  ~CryptAnalysis();

  // Run kasiski analysis and store the result internally
  void kasiski_analysis();
  // Guess the best key length and produces the plaintexts
  std::string crack();
};

void kasiski_analysis(const std::string &ciphertext);
std::string crack(const std::string &ciphertext, const int key_len);

char forward(char m, int amount);
char backward(char c, int amount);