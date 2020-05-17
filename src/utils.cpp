#include "utils.h"
#include <math.h>

std::unique_ptr<std::vector<bool>> generate_random_vector(const double p_success, const size_t simulation_size) {
  std::vector<bool> res;
  res.reserve(simulation_size);
  const int threshold = std::floor(p_success * RAND_MAX + 0.5);
  for (size_t i = 0; i < simulation_size; i++) {
    res.push_back(rand() > threshold);
  }
  return std::make_unique<std::vector<bool>>(std::vector<bool>(res));
}