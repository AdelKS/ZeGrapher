#include "Utils/algos.h"

#include "utils.h"

int main()
{
  using zg::utils::binomial_coefficient;

  constexpr uint validity_limit = 47;

  // symmetry C(n, k) == C(n, n - k)
  for (unsigned n = 0; n <= validity_limit; n++)
    for (unsigned k = 0; k <= n; k++)
      test(binomial_coefficient(n, k) == binomial_coefficient(n, n - k));

  // exactness against Pascal's triangle for every (n, k) up to n = 47,
  // the range over which the gamma-identity form is exact
  double pascal[validity_limit + 1][validity_limit + 1] = {};
  for (unsigned n = 0; n <= validity_limit; n++)
  {
    pascal[n][0] = pascal[n][n] = 1;
    for (unsigned k = 1; k < n; k++)
      pascal[n][k] = pascal[n - 1][k - 1] + pascal[n - 1][k];
  }

  for (unsigned n = 0; n <= validity_limit; n++)
    for (unsigned k = 0; k <= n; k++)
      test(binomial_coefficient(n, k) == pascal[n][k]);

  return 0;
}
