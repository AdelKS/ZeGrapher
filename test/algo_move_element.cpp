#include "Utils/algos.h"

#include "utils.h"

int main()
{
  std::vector<int> vals = {0, 1, 2, 3, 4};

  zg::utils::move_element(vals, 1, 3);
  test(bool(vals == std::vector<int>({0, 2, 3, 1, 4})));

  zg::utils::move_element(vals, 4, 0);
  test(bool(vals == std::vector<int>({4, 0, 2, 3, 1})));

  zg::utils::move_element(vals, 2, 2);
  test(bool(vals == std::vector<int>({4, 0, 2, 3, 1})));

  return 0;
}
