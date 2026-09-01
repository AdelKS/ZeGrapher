#include "Utils/donation.h"

#include "Utils/resources.h"
#include "Utils/yaml.h"

#include <QLocale>

namespace zg {

/// @brief the only key of website/content/<lang>/strings.yaml that the app reads
///
/// It sits outside the anonymous namespace because glaze reads field names by
/// reflection, which needs a type with linkage.
struct WebsiteStrings
{
  /// @brief the donation appeal, in markdown
  std::string donation;
};

QString Donation::words(int language) const
{
  const std::string yaml =
    readTextFile(websiteFolder(QLocale::Language(language)) + "/strings.yaml").toStdString();

  WebsiteStrings strings;
  if (glz::read_yaml<yml::skip_unknown_keys>(strings, yaml))
    return {};

  return QString::fromStdString(strings.donation).trimmed();
}

}
