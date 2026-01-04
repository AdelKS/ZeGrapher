#include <cstdlib>

inline void test(bool cond)
{
  if (not cond)
    std::exit(1);
}
