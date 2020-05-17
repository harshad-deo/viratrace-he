#include "gtest/gtest.h"
#include <iostream>
#include <stdlib.h>

// const int input_size = 4096 * 4096 + 1;
// const double _eps = 5e-14;

// double cnorm(const double arg) {
//   return 0.5 * erfc(-arg * M_SQRT1_2);
// }

TEST(VT_HE, EQUIV) {
  // auto input = std::vector<BlackVectorInput>();
  // BlackVector blackVector(input_size);
  // srand (time(NULL));
  // for(unsigned int i = 0; i < input_size; i++){
  //   const double spot = 64 + (rand() % 2048 - 1024) / 4096.0;
  //   const double strike = 64 + (rand() % 2048 - 1024) / 256.0;
  //   const double rd = 0.1 + (rand() % 2048 - 1024) / 40960.0;
  //   const double rf = 0.01 + (rand() % 2048 - 1024) / 81960.0;
  //   const double tau = 1 + (rand() % 2048 - 1024) / 4096.0;
  //   const double sigma = 0.1 + (rand() % 2048 - 1024) / 40960.0;
  //   BlackVectorInput inp(spot, strike, rd, rf, tau, sigma);
  //   input.push_back(inp);
  //   blackVector.add(i, inp);
  // }
  // blackVector.compute();
  // for(int i = 0; i < input_size; i++){
  //   auto arg = input[i];
  //   auto res = blackVector.result(i);
  //   const double spot = arg.getSpot();
  //   const double strike = arg.getStrike();
  //   const double rd = arg.getRd();
  //   const double rf = arg.getRf();
  //   const double tau = arg.getTau();
  //   const double sigma = arg.getSigma();
  //   const double d1 = (log(spot / strike) + (rd - rf + sigma * sigma / 2) * tau) / (sigma * sqrt(tau));
  //   const double d2 = (log(spot / strike) + (rd - rf - sigma * sigma / 2) * tau) / (sigma * sqrt(tau));
  //   const double delta = cnorm(d1) * exp(-rf * tau);
  //   const double value = delta * spot - strike * cnorm(d2) * exp(-rd * tau);
  //   const double errorValue = std::abs(res.getValue() - value);
  //   const double errorDelta = std::abs(res.getDelta() - delta);
  //   // printf("%d: calcValue: %g, calcDelta: %g\n", i, res.getValue(), res.getDelta());
  //   // printf("%d: errorValue: %g, errorDelta: %g, calcValue: %g, calcDelta: %g\n", i, errorValue, errorDelta,
  //   res.getValue(), res.getDelta()); ASSERT_LE(errorValue, _eps); ASSERT_LE(errorDelta, _eps);
  // }
}

int main(int argc, char **argv) {
  std::cout << "Testing chaloo ki jaye..." << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  const int res = RUN_ALL_TESTS();
  std::cout << "ho gaya" << std::endl;
  return res;
}