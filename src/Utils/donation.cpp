#include "Utils/donation.h"

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

void Donation::setLanguage(int lang)
{
  if (language == lang)
    return;

  language = lang;
  emit wordsChanged();
}

QString Donation::getWords() const
{
  const std::string yaml =
    readTextFile(websiteFolder(QLocale::Language(language)) + "/strings.yaml").toStdString();

  WebsiteStrings strings;
  if (glz::read_yaml(strings, yaml))
    return {};

  return QString::fromStdString(strings.donation).trimmed();
}

}
