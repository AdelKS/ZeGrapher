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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Utils/versionparser.h"
#include "buildconfig.h"

#include "utils.h"

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

  // a version without a patch number gets a patch of zero
  auto minor = parse_version_string("v3.1");

  test(minor.has_value());
  test((*minor == Version{3, 1, 0, FINAL_STAGE, 0, 0}));
  test(*minor < *res1);

  // a pre-release, with and without the tag's leading 'v'
  auto beta = parse_version_string("v4.0.0_beta2");

  test(beta.has_value());
  test((*beta == Version{4, 0, 0, 1, 2, 0}));
  test(parse_version_string("4.0.0_beta2") == beta);

  // the stages come in this order, and the release comes after all three
  auto alpha = parse_version_string("v4.0.0_alpha9");
  auto rc = parse_version_string("v4.0.0_rc0");

  test(alpha.has_value() and rc.has_value());
  test(*alpha < *beta);
  test(*beta < *rc);
  test(*rc < *res2);

  // a pre-release of 4.0.0 comes after every version of 3.1.1
  test(*res1 < *alpha);

  // a build between two releases comes just before the release it names
  auto dev = parse_version_string("4.0.0_rc0-dev");

  test(dev.has_value());
  test((*dev == Version{4, 0, 0, 2, 0, DEV_REVISION}));
  test(*beta < *dev);
  test(*dev < *rc);

  // the pattern is anchored: a version inside other text doesn't match
  test(not parse_version_string("ZeGrapher 4.0.0").has_value());
  test(not parse_version_string("4.0.0 and more").has_value());
  test(not parse_version_string("v4.0.0_gamma1").has_value());

  return 0;
}
