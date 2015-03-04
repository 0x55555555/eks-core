#include <gtest/gtest.h>
#include "Containers/XAlgorithm.h"

TEST (AlgorithmTest, removeIf) {
  std::vector<int> a = { 1, 2, 2, 1, 5, 3, 2, 4, 5 };
  
  Eks::Algorithm::removeIf(a, [](const auto &val) { return val == 2; });
  
  const std::vector<int> b = { 1, 1, 5, 3, 4, 5 };
  EXPECT_EQ(a, b);
}