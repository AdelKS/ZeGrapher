#include "Utils/algos.h"

#include <cassert>

int main()
{
  std::unordered_set<int> vals = {0,1,2,3,4, 5, 6, 7};

  zg::utils::erase(vals, [](int x){ return x >= 2; });

  assert(bool(vals == std::unordered_set<int>({0, 1})));

  return 0;
}
