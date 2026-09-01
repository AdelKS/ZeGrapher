#include "Utils/whatsnew.h"

#include "structures.h"

namespace zg {

namespace {

/// @brief the path appdata/release-notes.qrc.in gives to the YAML file that
///        appdata/releases.py writes
const QString notesFile = ":/releases/notes.yaml";

}

WhatsNew::WhatsNew(QObject* parent): QObject(parent)
{
  load();
}

void WhatsNew::setSince(QString version)
{
  if (since == version)
    return;

  since = std::move(version);
  load();
}

void WhatsNew::setEveryRelease(bool every)
{
  if (everyRelease == every)
    return;

  everyRelease = every;
  load();
}

void WhatsNew::load()
{
  entries.clear();

  const QString notes = readTextFile(notesFile);

  for (const Release& release: everyRelease ? allReleases(notes)
                                            : releasesSince(notes, since))
    entries.append(QVariantMap{{"tag", release.tag},
                               {"span", release.span},
                               {"summary", release.summary},
                               {"released", release.released}});

  emit entriesChanged();
}

}
