#include "gtest/gtest.h"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "Testing chaloo ki jaye..." << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  const int res = RUN_ALL_TESTS();
  std::cout << "ho gaya" << std::endl;
  return res;
}