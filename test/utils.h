#include <cstdlib>
#include <iostream>
#include <source_location>

inline void test(bool cond, std::source_location where = std::source_location::current())
{
  if (not cond)
  {
    std::cerr << where.file_name() << ':' << where.line() << ": test failed\n";
    std::exit(1);
  }
}
