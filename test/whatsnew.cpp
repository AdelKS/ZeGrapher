#include "Utils/whatsnew.h"

#include "utils.h"

// what appdata/releases.py writes for two headings: '## v4.1.0 (2026-10-02)'
// and '## v3.1.1 - v4.0.0 (2026-09-01)'
const QString notes = R"(- newest: v4.1.0
  span: ''
  summary: What this one brings.
- newest: v4.0.0
  span: v3.1.1 - v4.0.0
  summary: Complete rewrite.
)";

int main()
{
  // every call names its build, so the test does not depend on SOFTWARE_VERSION
  const zg::Build dev("4.0.0_rc0-dev");
  const zg::Build rc("4.0.0_rc0");
  const zg::Build full("4.0.0");

  // a first start shows the newest entry alone
  const auto first = zg::releasesSince(notes, "", dev);

  test(first.size() == 1);
  test(first[0].tag == "v4.1.0");
  // a heading of one tag has no span
  test(first[0].span.isEmpty());
  test(first[0].summary == "What this one brings.");

  // the releases over the one the reader had, the span of them as one entry
  const auto since = zg::releasesSince(notes, "v3.1.1", dev);

  test(since.size() == 2);
  test(since[1].tag == "v4.0.0");
  test(since[1].span == "v3.1.1 - v4.0.0");
  test(since[1].summary == "Complete rewrite.");

  // a version that a span holds keeps that span: the reader saw none of it
  const auto cut = zg::releasesSince(notes, "v4.0.0_beta1", dev);

  test(cut.size() == 2);
  test(cut[1].tag == "v4.0.0");
  test(cut[1].span == "v3.1.1 - v4.0.0");

  // a reader who ran v4.0.0 sees v4.1.0 alone
  test(zg::releasesSince(notes, "v4.0.0", dev).size() == 1);

  // the newest version has nothing to show
  test(zg::releasesSince(notes, "v4.1.0", dev).isEmpty());

  // a reader who already ran the build has nothing to see. Without this the
  // span that leads to v4.0.0 would come back at every start, because every
  // pre-release of 4.0.0 sits under that tag
  test(zg::releasesSince(notes, "4.0.0_rc0-dev", dev).isEmpty());
  test(zg::releasesSince(notes, "v4.0.0_rc0", dev).isEmpty());
  test(zg::releasesSince(notes, "v4.0.0_rc0", rc).isEmpty());
  test(zg::releasesSince(notes, "v4.0.0", full).isEmpty());

  // v4.0.0 and v4.1.0 both come after the dev build, so neither has a page on
  // GitHub and neither card shows its button
  const auto all = zg::allReleases(notes, dev);

  test(all.size() == 2);
  test(all[0].tag == "v4.1.0");
  test(all[1].tag == "v4.0.0");
  test(not all[0].released);
  test(not all[1].released);

  // the release build has the page of its own release
  const auto allFull = zg::allReleases(notes, full);

  test(not allFull[0].released);
  test(allFull[1].released);

  // a release under the build is out
  test(zg::isReleased(*parse_version_string("v3.1.1"), dev));
  test(zg::isReleased(*parse_version_string("v4.0.0_alpha1"), dev));

  // a '-dev' build comes before the release it names, so that one is not out
  test(not zg::isReleased(*parse_version_string("v4.0.0_rc0"), dev));

  // a release build names a release that is out
  test(zg::isReleased(*parse_version_string("v4.0.0_rc0"), rc));
  test(not zg::isReleased(*parse_version_string("v4.0.0"), rc));
  test(zg::isReleased(*parse_version_string("v4.0.0"), full));

  return 0;
}
