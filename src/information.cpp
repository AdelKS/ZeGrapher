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
#include "Utils/languages.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <glaze/yaml.hpp>

namespace {

/// @brief the document of the last run, empty when no run wrote one
QString lastDocumentPath()
{
  QString path = QStandardPaths::locate(QStandardPaths::AppConfigLocation,
                                        Information::lastDocumentName);

  // ZeGrapher 4.0.0_beta3 and older wrote the document under this name
  if (path.isEmpty())
    path = QStandardPaths::locate(QStandardPaths::AppConfigLocation, "last-workbook.zg");

  return path;
}

}

Information::Information(QObject* parent):
  QObject(parent), appSettings(this), graphSettings(this)
{
  // the settings file can name another language
  appSettings.language = systemLanguage();

  restoreSettings();
}

Information::~Information()
{
  saveSettings();
  saveLastDocument();
}

void Information::saveSettings()
{
  const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  if (not folder.isEmpty())
    writeYaml(QUrl::fromLocalFile(folder + '/' + settingsName),
              {.zegrapher = SOFTWARE_VERSION,
               .math_objects = {},
               .graph = {},
               .app = appSettings.exportPod()});
}

void Information::restoreSettings()
{
  QFile file(QStandardPaths::locate(QStandardPaths::AppConfigLocation, settingsName));
  if (not file.open(QIODevice::ReadOnly))
    return;

  const QByteArray bytes = file.readAll();

  // a newer version of ZeGrapher can write keys that this one does not know
  POD settings;
  if (glz::read_yaml<glz::yaml::yaml_opts{.error_on_unknown_keys = false}>(
        settings, std::string_view(bytes.data(), bytes.size())))
    return;

  lastRunVersion = QString::fromStdString(settings.zegrapher.value_or(""));

  if (settings.app)
    appSettings.importPod(std::move(*settings.app));
}

void Information::saveLastDocument()
{
  const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  if (not folder.isEmpty())
    exportYaml(QUrl::fromLocalFile(folder + '/' + lastDocumentName));
}

bool Information::restoreLastDocument()
{
  const QString path = lastDocumentPath();
  if (path.isEmpty())
    return false;

  importYaml(QUrl::fromLocalFile(path));

  return true;
}

void Information::loadExampleDocument()
{
  auto* cst = zg::mathWorld.addMathObject(zg::MathObject::CONSTANT)->getConstant();
  cst->set_value(2);
  cst->setName("a");

  auto* cos = zg::mathWorld.addMathObject(zg::MathObject::EQUATION)->getEquation();
  cos->setEquation("f(x) = a * cos(x)");

  auto* fibo = zg::mathWorld.addMathObject(zg::MathObject::EQUATION)->getEquation();
  fibo->setEquation("u(n) = a ; a ; u(n-2) + u(n-1)");
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

void Information::exportFailed(const QString& file, const QString& details)
{
  appendIoErr({.title = tr("Could not export the graph"),
               .file = file,
               .text = tr("Could not write the file."),
               .details = details});
}

void Information::readFailed(const QString& file, const QString& details)
{
  appendIoErr({.title = tr("Could not read the file"),
               .file = file,
               .text = tr("Could not open the file for reading."),
               .details = details});
}

void Information::exportYaml(QUrl filename)
{
  writeYaml(filename, {.zegrapher = SOFTWARE_VERSION,
                       .math_objects = zg::mathWorld.exportPod(),
                       .graph = graphSettings.exportPod(),
                       .app = {}});
}

void Information::writeYaml(QUrl filename, const POD& pod)
{
  qDebug() << "Exporting to " << filename.toLocalFile();

  auto exp_content = glz::write_yaml(pod);
  if (not exp_content)
  {
    appendIoErr(
      {.title = tr("Internal bug"),
       //: %1 is the page of the issues on GitHub
       .text = tr("Please report this bug to contact@zegrapher.com or %1, with "
                  "a way to reproduce it.").arg(REPOSITORY_URL "/issues"),
       .details = tr("Could not serialize ZeGrapher's state")});
    return;
  }

  const QString path = filename.toLocalFile();
  if (path.isEmpty())
  {
    appendIoErr({
      .title = tr("Could not save the document"),
      .file = filename.toString(),
      .text = tr("The target file path is invalid."),
    });
    return;
  }

  QFileInfo info(path);
  if (not info.absoluteDir().mkpath("."))
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("Could not create the folder."),
                 .details = info.absolutePath()});
    return;
  }

  QSaveFile file(path);
  if (not file.open(QIODevice::WriteOnly))
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("Could not open the file for writing."),
                 .details = file.errorString()});
    return;
  }

  qint64 written = file.write(exp_content->data(), exp_content->size());
  if (written != qint64(exp_content->size()))
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("The write did not finish. The file is left unchanged."),
                 .details = file.errorString()});
    return;
  }

  if (not file.commit())
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("Could not replace the file with the new version."),
                 .details = file.errorString()});
    return;
  }
}

void Information::importYaml(QUrl filename)
{
  qInfo() << "Importing from " << filename.toLocalFile();

  const QString path = filename.toLocalFile();
  if (path.isEmpty())
  {
    appendIoErr({.title = tr("Could not load the document"),
                 .file = filename.toString(),
                 .text = tr("The path to the file has the wrong format.")});
    return;
  }

  QFile file(path);
  if (not file.exists())
  {
    appendIoErr({.title = tr("Could not load the document"),
                 .file = path,
                 .text = tr("The file does not exist.")});
    return;
  }

  if (not file.open(QIODevice::ReadOnly))
  {
    appendIoErr({.title = tr("Could not load the document"),
                 .file = path,
                 .text = tr("Could not open the file for reading.")});
    return;
  }

  const QByteArray bytes = file.readAll();
  if (file.error() != QFileDevice::NoError)
  {
    appendIoErr({.title = tr("Could not load the document"),
                 .file = path,
                 .text = tr("Could not read the whole file."),
                 .details = file.errorString()});
    return;
  }

  const std::string_view content(bytes.data(), bytes.size());

  POD pod;
  auto read_error = glz::read_yaml(pod, content);
  if (read_error)
    appendIoErr({.title = tr("Could not load the document"),
                 .file = path,
                 .text = tr("The file is not a valid ZeGrapher document."),
                 .details = QString::fromStdString(glz::format_error(read_error, content))});

  else {
    if (pod.graph) graphSettings.importPod(std::move(*pod.graph));
    if (pod.math_objects) zg::mathWorld.importPod(std::move(*pod.math_objects));
  }
}
