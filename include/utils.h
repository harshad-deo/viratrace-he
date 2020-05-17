#include <memory>
#include <vector>

#ifndef VTHE_UTILS
#define VTHE_UTILS

std::unique_ptr<std::vector<bool>> generate_random_vector(const double p_success, const size_t simulation_size);

#endif