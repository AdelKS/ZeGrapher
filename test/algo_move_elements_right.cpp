#include "Utils/algos.h"

#include "utils.h"

int main()
{
  std::vector<int> vals = {0,1,2,3,4};

  zg::utils::move_elements_right(vals, 2);

  test(bool(vals == std::vector<int>({0, 1, 0, 1, 2, 3, 4})));

  return 0;
}
