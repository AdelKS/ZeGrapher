#include "Calculus/function.h"
#include "structures.h"

#include <chrono>
#include <iostream>

using namespace std::chrono;

int main()
{
  {
    Function f(0, "foo");
    f.validateExpression("3*cos(k*x) + 2*sin(x/k) + 4");

    double t = 1;

    double x = 0;
    auto begin = high_resolution_clock::now();
    double res = 0;
    size_t iterations = 0;
    while (high_resolution_clock::now() - begin < 1s)
    {
      res += f.getFuncValue(x, t);
      iterations++;
      x++;
      t++;
    }
    auto end = high_resolution_clock::now();
    std::cout << "Avg zg::Function eval time: "
              << duration_cast<nanoseconds>((end - begin) / iterations).count() << "ns"
              << std::endl;
    std::cout << "dummy val: " << res << std::endl;
  }
  {
    double cpp_t = 1;
    auto cpp_f = [&](double x) {
      return 3*cos(cpp_t*x) + 2*sin(x/cpp_t) + 4;
    };

    double x = 0;
    auto begin = high_resolution_clock::now();
    double res = 0;
    size_t iterations = 0;
    while (high_resolution_clock::now() - begin < 1s)
    {
      res += cpp_f(x);
      iterations++;
      x++;
      cpp_t++;
    }
    auto end = high_resolution_clock::now();
    std::cout << "Avg C++ function eval time: " << duration_cast<nanoseconds>((end - begin)/iterations).count() << "ns" << std::endl;
    std::cout << "dummy val: " << res << std::endl;

  }

  return 0;
}
