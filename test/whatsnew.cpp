#include "Utils/whatsnew.h"

#include "utils.h"

const QString notes = R"(# Release notes

## v4.1.0 (2026-10-02)

What this one brings.

## v3.1.1 - v4.0.0 (2026-09-01)

Complete rewrite.
)";

int main()
{
  // a first start shows the newest entry alone
  const auto first = zg::releasesSince(notes, "");

  test(first.size() == 1);
  test(first[0].tag == "v4.1.0");
  // a heading of one tag repeats the tag, and the panel then draws it once.
  // The day of the heading stays out of what the panel draws
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

  const auto all = zg::allReleases(notes);

  test(all.size() == 2);
  test(all[0].tag == "v4.1.0");
  test(all[1].tag == "v4.0.0");

  return 0;
}
