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

#include "Utils/releasenotes.h"

#include "utils.h"

// an announcement about v4.1.0 alone, one about the span that leads to v4.0.0,
// and two changes entries in that span
const QString notes = R"(releases:
  - tag: v4.1.0
    date: 2026-10-02
announcements:
  - to: v4.1.0
    text: What this one brings.
  - from: v3.1.1
    to: v4.0.0
    text: |
      Complete rewrite.
changes:
  - from: v4.0.0_rc1
    to: v4.0.0
    text: Fixed the PDF export.
  - from: v4.0.0_rc0
    to: v4.0.0_rc1
    text: Fixed the SVG export.
)";

int main()
{
  // every call names its build, so the test does not depend on SOFTWARE_VERSION
  const auto dev = parse_version_string("4.0.0_rc0-dev");
  const auto rc0 = parse_version_string("4.0.0_rc0");
  const auto rc1 = parse_version_string("4.0.0_rc1");
  const auto full = parse_version_string("4.0.0");
  const auto next = parse_version_string("4.1.0");

  // a first start only shows the newest entry
  const auto first = zg::releasesSince(notes, "", next);

  test(first.size() == 1);
  test(first[0].tag == "v4.1.0");
  // an entry without 'from' has no span
  test(first[0].span.isEmpty());
  test(first[0].summary == "What this one brings.");

  // the entries newer than the user's version. An announcement comes above the
  // changes entry that ends on the same release
  const auto since = zg::releasesSince(notes, "v3.1.1", next);

  test(since.size() == 4);
  test(since[1].tag == "v4.0.0");
  test(since[1].span == "v3.1.1 - v4.0.0");
  test(since[1].summary == "Complete rewrite.");
  test(since[2].span == "v4.0.0_rc1 - v4.0.0");
  test(since[3].span == "v4.0.0_rc0 - v4.0.0_rc1");

  // an update from rc0 to rc1 shows the announcement that rc1 sits under, and
  // what rc1 changed
  const auto update = zg::releasesSince(notes, "v4.0.0_rc0", rc1);

  test(update.size() == 2);
  test(update[0].span == "v3.1.1 - v4.0.0");
  test(update[1].span == "v4.0.0_rc0 - v4.0.0_rc1");

  // a user who ran v4.0.0 only sees v4.1.0
  test(zg::releasesSince(notes, "v4.0.0", next).size() == 1);

  // the newest version has nothing to show
  test(zg::releasesSince(notes, "v4.1.0", next).isEmpty());

  // a user who already ran the build, or a newer one, sees nothing. Without
  // this, the announcement that leads to v4.0.0 would come back at every start,
  // because every 4.0.0 pre-release sits under it
  test(zg::releasesSince(notes, "4.0.0_rc0-dev", dev).isEmpty());
  test(zg::releasesSince(notes, "v4.0.0_rc0", dev).isEmpty());
  test(zg::releasesSince(notes, "v4.0.0_rc0", rc0).isEmpty());
  test(zg::releasesSince(notes, "v4.0.0", full).isEmpty());

  // rc0 only shows the announcement: the changes entries start at rc0 or
  // later, and the v4.1.0 announcement is about v4.1.0 alone
  const auto allRc0 = zg::allReleases(notes, rc0);

  test(allRc0.size() == 1);
  test(allRc0[0].tag == "v4.0.0");
  // v4.0.0 comes after rc0, so it has no GitHub page and its card shows no
  // button
  test(not allRc0[0].released);

  // the release build has its own release page, and leaves out v4.1.0
  const auto allFull = zg::allReleases(notes, full);

  test(allFull.size() == 3);
  test(allFull[0].released);

  // a release older than the build is out
  test(zg::isReleased(*parse_version_string("v3.1.1"), dev));
  test(zg::isReleased(*parse_version_string("v4.0.0_alpha1"), dev));

  // a '-dev' build comes before the release it names, so that one is not out
  test(not zg::isReleased(*parse_version_string("v4.0.0_rc0"), dev));

  // a release build names a release that is out
  test(zg::isReleased(*parse_version_string("v4.0.0_rc0"), rc0));
  test(not zg::isReleased(*parse_version_string("v4.0.0"), rc0));
  test(zg::isReleased(*parse_version_string("v4.0.0"), full));

  return 0;
}
