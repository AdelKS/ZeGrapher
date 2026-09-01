#include "Utils/whatsnew.h"

#include "structures.h"

#include <glaze/yaml.hpp>

namespace zg {

/// @brief website/content/<lang>/strings.yaml, as the site writes it
///
/// It sits outside the anonymous namespace because glaze reads the name of each
/// field by reflection, which needs a type with linkage.
struct WebsiteStrings
{
  /// @brief the words of each entry of the footer of the site, which the app
  ///        does not show
  std::map<std::string, std::string> footer;

  /// @brief the markdown that asks for a donation
  std::string donation;
};

namespace {

/// @brief appdata/meson.build writes this file under this prefix
const QString notesFile = ":/releases/notes.md";

/// @brief the file of website/content/<lang>/ that holds the short strings of
///        that language, the words that ask for a donation among them
const QString stringsFile = "strings.yaml";

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
  const std::string yaml =
    readTextFile(websiteFolder(QLocale::Language(language)) + '/' + stringsFile).toStdString();

  WebsiteStrings strings;
  if (glz::read_yaml(strings, yaml))
    return {};

  return QString::fromStdString(strings.donation).trimmed();
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
                               {"summary", release.summary},
                               {"released", release.released}});

  emit entriesChanged();
}

}
