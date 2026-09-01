#include "Utils/whatsnew.h"

#include "utils.h"

// what appdata/releases.py writes for two headings: '## v4.1.0 (2026-10-02)'
// and '## v3.1.1 - v4.0.0 (2026-09-01)'
const QString notes = R"(- after: v4.1.0
  newest: v4.1.0
  summary: What this one brings.
- after: v3.1.1
  newest: v4.0.0
  summary: Complete rewrite.
)";

int main()
{
  // a first start shows the newest entry alone
  const auto first = zg::releasesSince(notes, "");

  test(first.size() == 1);
  test(first[0].tag == "v4.1.0");
  // a heading of one tag repeats the tag, and the panel then draws it once
  test(first[0].span == "v4.1.0");
  test(first[0].summary == "What this one brings.");

  // the releases over the one the reader had, the span of them as one entry
  const auto since = zg::releasesSince(notes, "v3.1.1");

  test(since.size() == 2);
  test(since[1].tag == "v4.0.0");
  test(since[1].span == "v3.1.1 - v4.0.0");
  test(since[1].summary == "Complete rewrite.");

  // a version that a span holds keeps that span: the reader saw none of it
  const auto cut = zg::releasesSince(notes, "v4.0.0_beta1");

  test(cut.size() == 2);
  test(cut[1].tag == "v4.0.0");
  test(cut[1].span == "v3.1.1 - v4.0.0");

  // the release that a span counts from stays out of it
  test(zg::releasesSince(notes, "v4.0.0").size() == 1);

  // the newest version has nothing to show
  test(zg::releasesSince(notes, "v4.1.0").isEmpty());

  // this build is 4.0.0_rc0-dev, and a reader who already ran it has nothing
  // to see. Without this the span that leads to v4.0.0 would come back at
  // every start, because every pre-release of 4.0.0 sits under that tag
  test(zg::releasesSince(notes, "4.0.0_rc0-dev").isEmpty());
  test(zg::releasesSince(notes, "v4.0.0_rc0").isEmpty());

  const auto all = zg::allReleases(notes);

  test(all.size() == 2);
  test(all[0].tag == "v4.1.0");
  test(all[1].tag == "v4.0.0");

  // v4.0.0 and v4.1.0 both come after this build, so neither has a page on
  // GitHub and neither card shows its button
  test(not all[0].released);
  test(not all[1].released);

  // a release under this build is out
  test(zg::isReleased(*parse_version_string("v3.1.1")));
  test(zg::isReleased(*parse_version_string("v4.0.0_alpha1")));

  // a '-dev' build comes before the release it names, so that one is not out
  test(not zg::isReleased(*parse_version_string("v4.0.0_rc0")));

  return 0;
}
