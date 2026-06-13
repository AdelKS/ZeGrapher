/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "information.h"
#include "information.h"

#include <QStandardPaths>
#include <glaze/yaml.hpp>

Information::Information(QObject* parent):
  QObject(parent), appSettings(this), graphSettings(this)
{}

void Information::screenChanged(QWindow* win)
{

  QSizeF pixelSize = win->screen()->size().toSizeF();
  QSizeF cmSize = win->screen()->physicalSize() / 10;

  qDebug() << "Monitor pixel size: " << pixelSize;
  qDebug() << "Monitor cm size: " << cmSize;

  double px_per_cm = sqrt(pixelSize.width() * pixelSize.height() / (cmSize.width() * cmSize.height()));

  if (pixelDensity != px_per_cm)
  {
    qDebug() << "pixel density " << px_per_cm << "px per cm";
    pixelDensity = px_per_cm;
    emit pixelDensityChanged();
  }
}

QString Information::exportYaml(QUrl filename)
{
  qInfo() << "Exporting to " << filename.toLocalFile();
  POD pod = {
    .math_objects = zg::mathWorld.exportPod(),
    .graph = graphSettings.exportPod(),
    .app = appSettings.exportPod()
  };

  auto write_error = glz::write_file_yaml(pod, filename.toLocalFile().toStdString());
  if (write_error) {
    return QString::fromStdString(glz::format_error(write_error));
  }
  else return {};
}

template <typename T>
struct P {};

QString Information::importYaml(QUrl filename)
{
  qInfo() << "Importing from " << filename.toLocalFile();

  QString error;

  constexpr glz::yaml::yaml_opts opts = {.error_on_unknown_keys = false };

  // we read only the math settings then only the graph settings
  // se we can do partial loads if ever.

  const std::string path = filename.toLocalFile().toStdString();

  auto partialImport = [&]<typename Pod>(P<Pod>, auto& importInto) {
    Pod pod;
    auto& [element] = pod;
    auto read_error = glz::read_file_yaml<opts>(pod, path);
    if (read_error)
      error += (error.isEmpty() ? "" : "\n") + QString::fromStdString(glz::format_error(read_error));
    else if (element)
      importInto.importPod(std::move(*element));
  };

  partialImport(P<PartialMathPOD>{}, zg::mathWorld);
  partialImport(P<PartialGraphPOD>{}, graphSettings);
  partialImport(P<PartialAppPOD>{}, appSettings);

  return error;
}
