#include "Utils/versionparser.h"

#include "utils.h"

int main()
{
  auto res1 = parse_version_string("v3.1.1-r111");

  test(res1.has_value());
  test((*res1 == std::array<ushort, 4>{3, 1, 1, 111}));

  auto res2 = parse_version_string("v4.0.0");

  test(res2.has_value());
  test((*res2 == std::array<ushort, 4>{4, 0, 0, 0}));

  auto res3 = parse_version_string("v4.0.0-r1");

  test(res3.has_value());
  test((*res3 == std::array<ushort, 4>{4, 0, 0, 1}));

  auto res4 = parse_version_string("foo");

  test(not res4.has_value());

  auto res5 = parse_version_string(SOFTWARE_VERSION);

  test(res5.has_value());

  return 0;
}
