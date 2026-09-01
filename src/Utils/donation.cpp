#include "Utils/donation.h"

#include "Utils/resources.h"

#include <glaze/yaml.hpp>

namespace zg {

/// @brief the one key of website/content/<lang>/strings.yaml that the app reads
///
/// It sits outside the anonymous namespace because glaze reads the name of each
/// field by reflection, which needs a type with linkage.
struct WebsiteStrings
{
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
  if (glz::read_yaml<glz::yaml::yaml_opts{.error_on_unknown_keys = false}>(strings, yaml))
    return {};

  return QString::fromStdString(strings.donation).trimmed();
}

}
