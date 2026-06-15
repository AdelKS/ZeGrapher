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

IOError Information::popIoError()
{
  IOError err = std::move(ioErrors.front());
  ioErrors.pop_front();
  return err;
}

void Information::appendIoErr(IOError err)
{
  ioErrors.push_back(std::move(err));
  emit ioErrorCountChanged();
}

void Information::exportYaml(QUrl filename)
{
  qDebug() << "Exporting to " << filename.toLocalFile();
  POD pod = {
    .math_objects = zg::mathWorld.exportPod(),
    .graph = graphSettings.exportPod(),
    .app = appSettings.exportPod()
  };

  auto exp_content = glz::write_yaml(pod);
  if (not exp_content)
  {
    appendIoErr(
      {.title = tr("Internal bug"),
       .text = tr("This is a bug, please report it to "
                  "contact@zegrapher.com or at "
                  "https://github.com/AdelKS/ZeGrapher/issues with a way on how to reproduce it."),
       .details = tr("Could not serialize ZeGrapher's state")});
    return;
  }

  const QString path = filename.toLocalFile();
  if (path.isEmpty())
  {
    appendIoErr({
      .title = tr("File information error"),
      .file = filename.toString(),
      .text = tr("Input is not a local file path."),
    });
    return;
  }

  QFileInfo info(path);
  if (not info.absoluteDir().mkpath("."))
  {
    appendIoErr({.title = tr("File operation error"),
                 .file = path,
                 .text = tr("Could not create directory."),
                 .details = info.absolutePath()});
    return;
  }

  QSaveFile file(path);
  if (not file.open(QIODevice::WriteOnly))
  {
    appendIoErr({.title = tr("File operation error"),
                 .file = path,
                 .text = tr("Could not open file"),
                 .details = file.errorString()});
    return;
  }

  qint64 written = file.write(exp_content->data(), exp_content->size());
  if (written != qint64(exp_content->size()))
  {
    appendIoErr({.title = tr("File operation error"),
                 .file = path,
                 .text = tr("Incomplete write of ZeGrapher document, operation cancelled."),
                 .details = file.errorString()});
    return;
  }

  if (not file.commit())
  {
    appendIoErr({.title = tr("File operation error"),
                 .file = path,
                 .text = tr("Could not finalize saved document."),
                 .details = file.errorString()});
    return;
  }
}

template <typename T>
struct P {};

void Information::importYaml(QUrl filename)
{
  qInfo() << "Importing from " << filename.toLocalFile();

  const QString path = filename.toLocalFile();
  if (path.isEmpty())
  {
    appendIoErr({.title = tr("File information error"),
                 .file = path,
                 .text = tr("Input is not a local file path."),
                 .details = filename.toString()});
    return;
  }

  QFile file(path);
  if (not file.open(QIODevice::ReadOnly))
  {
    appendIoErr({.title = tr("File operation error"),
                 .file = path,
                 .text = tr("Could not open file.")});
    return;
  }

  const QByteArray bytes = file.readAll();
  if (file.error() != QFileDevice::NoError)
  {
    appendIoErr({.title = tr("File operation error"),
                 .file = path,
                 .text = tr("Could not read file entirely."),
                 .details = file.errorString()});
  }

  const std::string_view content(bytes.data(), bytes.size());

  POD pod;
  auto read_error = glz::read_yaml(pod, content);
  if (read_error)
    appendIoErr({.title = tr("Parsing error"),
                 .file = path,
                 .text = tr("Error while parsing document."),
                 .details = QString::fromStdString(glz::format_error(read_error, content))});

  else {
    if (pod.app) appSettings.importPod(std::move(*pod.app));
    if (pod.graph) graphSettings.importPod(std::move(*pod.graph));
    if (pod.math_objects) zg::mathWorld.importPod(std::move(*pod.math_objects));
  }
}
