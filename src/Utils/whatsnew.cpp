#include "Utils/whatsnew.h"

#include "structures.h"

namespace zg {

namespace {

/// @brief appdata/meson.build writes this file under this prefix
const QString notesFile = ":/releases/notes.md";

/// @brief the file of website/content/<lang>/ that asks for a donation
const QString donationFile = "donation.md";

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

void WhatsNew::setLanguage(int lang)
{
  if (language == lang)
    return;

  language = lang;
  emit donationChanged();
}

QString WhatsNew::getDonation() const
{
  return readTextFile(websiteFolder(QLocale::Language(language)) + '/' + donationFile).trimmed();
}

QUrl WhatsNew::getDonationUrl() const
{
  return QUrl(DONATE_URL);
}

void WhatsNew::load()
{
  entries.clear();

  const QString notes = readTextFile(notesFile);

  for (const Release& release: everyRelease ? allReleases(notes)
                                            : releasesSince(notes, since))
    entries.append(QVariantMap{{"tag", release.tag},
                               {"span", release.span},
                               {"summary", release.summary}});

  emit entriesChanged();
}

}
