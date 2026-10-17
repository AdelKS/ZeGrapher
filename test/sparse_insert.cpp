#include "Utils/sampledcurve.h"
#include "utils.h"

int main()
{
  zg::SampledCurve curve {
    .input = {{1.}, {1.}, {1.}},
    .curve = {zg::real_pt{{1.}, {1.}}, {{1.}, {1.}}, {{1.}, {1.}}},
    .px_curve = {{1., 1.}, {1., 1.}, {1., 1.}},
  };

  curve.sparse_insert(
    {0, 1, 2, 3},
    {{2.}, {3.}, {4.}, {5.}},
    {zg::real_pt{{2.}, {2.}}, {{3.}, {3.}}, {{4.}, {4.}}, {{5.}, {5.}}},
    {{2., 2.}, {3., 3.}, {4., 4.}, {5., 5.}}
  );

  test(bool(curve.input == std::vector<zg::real_unit>{{2.}, {1.}, {3.}, {1.}, {4.}, {1.}, {5.}}));

  return 0;
}
