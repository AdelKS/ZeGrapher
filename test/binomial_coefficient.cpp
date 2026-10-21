/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Utils/algos.h"

#include "utils.h"

int main()
{
  using zg::utils::binomial_coefficient;

  constexpr unsigned validity_limit = 47;

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
