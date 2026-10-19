/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Utils/donation.h"

#include "Utils/resources.h"
#include "Utils/yaml.h"

#include <QLocale>
#include <glaze/yaml.hpp>

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
