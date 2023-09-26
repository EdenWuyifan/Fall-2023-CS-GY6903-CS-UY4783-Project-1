#ifndef COMMON_H__
#define COMMON_H__

// Common functions
#include <optional>
#include <utility>
#include <vector>

int diff(int c, int p);
int ctoi(char c);
bool sortByVal(const std::pair<std::size_t, std::size_t> &a,
               const std::pair<std::size_t, std::size_t> &b);
char forward(char m, int amount);

class Combination {
 private:
  std::size_t n, k;
  std::vector<std::size_t> state;
  int end;
  int current = 0;
  int round = 0;

 public:
  Combination(std::size_t n, std::size_t k);
  std::optional<std::vector<std::size_t>> next();

  std::size_t size() { return end; }
};

#endif  // COMMON_H__