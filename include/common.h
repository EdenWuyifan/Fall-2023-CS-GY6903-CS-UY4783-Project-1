#ifndef COMMON_H__
#define COMMON_H__

// Common functions
#include <utility>

int diff(int c, int p);
int ctoi(char c);
bool sortByVal(const std::pair<std::size_t, std::size_t> &a,
               const std::pair<std::size_t, std::size_t> &b);
char forward(char m, int amount);

#endif  // COMMON_H__