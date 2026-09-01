#include "Utils/whatsnew.h"

#include "Utils/resources.h"

namespace zg {

namespace {

/// @brief the path appdata/meson.build gives to the YAML file that
///        appdata/releases.py writes
const QString notesFile = ":/releases/notes.yaml";

}

void WhatsNew::setSince(QString version)
{
  if (since == version)
    return;

  since = std::move(version);
  emit entriesChanged();
}

void WhatsNew::setEveryRelease(bool every)
{
  if (everyRelease == every)
    return;

  everyRelease = every;
  emit entriesChanged();
}

QVariantList WhatsNew::getEntries() const
{
  const QString notes = readTextFile(notesFile);

  QVariantList entries;
  for (const Release& release: everyRelease ? allReleases(notes)
                                            : releasesSince(notes, since))
    entries.append(QVariantMap{{"tag", release.tag},
                               {"span", release.span},
                               {"summary", release.summary},
                               {"released", release.released}});

  return entries;
}

}
