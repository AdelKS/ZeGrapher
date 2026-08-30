#include "Utils/versionparser.h"

#include "utils.h"

#include <algorithm>

int main()
{
  auto res1 = parse_version_string("v3.1.1-r111");

  test(res1.has_value());
  test((*res1 == Version{3, 1, 1, FINAL_STAGE, 0, 111}));

  auto res2 = parse_version_string("v4.0.0");

  test(res2.has_value());
  test((*res2 == Version{4, 0, 0, FINAL_STAGE, 0, 0}));

  auto res3 = parse_version_string("v4.0.0-r1");

  test(res3.has_value());
  test((*res3 == Version{4, 0, 0, FINAL_STAGE, 0, 1}));

  auto res4 = parse_version_string("foo");

  test(not res4.has_value());

  auto res5 = parse_version_string(SOFTWARE_VERSION);

  test(res5.has_value());

  // a version that leaves its patch number out reads it as zero, and it comes
  // under every version that names one
  auto minor = parse_version_string("v3.1");

  test(minor.has_value());
  test((*minor == Version{3, 1, 0, FINAL_STAGE, 0, 0}));
  test(std::ranges::lexicographical_compare(*minor, *res1));

  // a pre-release, with and without the letter that opens a tag
  auto beta = parse_version_string("v4.0.0_beta2");

  test(beta.has_value());
  test((*beta == Version{4, 0, 0, 1, 2, 0}));
  test(parse_version_string("4.0.0_beta2") == beta);

  // the stages come in this order, and the release comes after all three
  auto alpha = parse_version_string("v4.0.0_alpha9");
  auto rc = parse_version_string("v4.0.0_rc0");

  test(alpha.has_value() and rc.has_value());
  test(std::ranges::lexicographical_compare(*alpha, *beta));
  test(std::ranges::lexicographical_compare(*beta, *rc));
  test(std::ranges::lexicographical_compare(*rc, *res2));

  // a pre-release of 4.0.0 comes after every version of 3.1.1
  test(std::ranges::lexicographical_compare(*res1, *alpha));

  return 0;
}
