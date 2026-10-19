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
#include "Utils/versionparser.h"
#include "buildconfig.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <glaze/yaml.hpp>

namespace {

/// @brief the name of the file saveLastDocument() writes, under AppConfigLocation
constexpr auto lastDocumentName = "last-document.zg";

/// @brief the name of the file saveSettings() writes, under AppConfigLocation
constexpr auto settingsName = "settings.yaml";

/// @brief the document of the last run, empty when no run wrote one
QString lastDocumentPath()
{
  QString path = QStandardPaths::locate(QStandardPaths::AppConfigLocation, lastDocumentName);

  // ZeGrapher 4.0.0_beta3 and older wrote the document under this name
  if (path.isEmpty())
    path = QStandardPaths::locate(QStandardPaths::AppConfigLocation, "last-workbook.zg");

  return path;
}

/// @brief the ZeGrapher version that wrote a document, when newer than this
///        build. A patch release keeps its minor release's format
std::optional<QString> newerWriter(std::string_view content)
{
  Information::HeaderPOD header;

  if (glz::read_yaml<zg::yml::skip_unknown_keys>(header, content) or not header.zegrapher)
    return {};

  const QString writer = QString::fromStdString(*header.zegrapher);
  const auto version = parse_version_string(writer);
  const auto build = build_version();
  if (not version or not build
      or std::pair((*version)[0], (*version)[1]) <= std::pair((*build)[0], (*build)[1]))
    return {};

  return writer;
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
              SettingsPOD{.zegrapher = SOFTWARE_VERSION, .app = appSettings.exportPod()});
}

void Information::restoreSettings()
{
  const QString path = QStandardPaths::locate(QStandardPaths::AppConfigLocation, settingsName);
  if (path.isEmpty())
    return;

  QFile file(path);
  if (not file.open(QIODevice::ReadOnly))
    return;

  const QByteArray bytes = file.readAll();

  SettingsPOD settings;
  if (glz::read_yaml<zg::yml::skip_unknown_keys>(settings, std::string_view(bytes.data(), bytes.size())))
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

  const qsizetype errorCount = ioErrors.size();
  importYaml(QUrl::fromLocalFile(path));

  if (ioErrors.size() != errorCount)
  {
    const QFileInfo info(path);
    const QString kept = info.dir().filePath(
      info.completeBaseName() + QDateTime::currentDateTime().toString("-yyyyMMdd-hhmmss.") + info.suffix());

    if (QFile::rename(path, kept))
      ioErrors.back().file = kept;
  }

  return true;
}

void Information::loadExampleDocument()
{
  auto* cst = zg::mathWorld->addMathObject(zg::MathObject::CONSTANT)->getConstant();
  cst->set_value(2);
  cst->setName("a");

  auto* cos = zg::mathWorld->addMathObject(zg::MathObject::EQUATION)->getEquation();
  cos->setEquation("f(x) = a * cos(x)");

  auto* fibo = zg::mathWorld->addMathObject(zg::MathObject::EQUATION)->getEquation();
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
  appendIoErr({.title = tr("Could not export the file"),
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

bool Information::exportYaml(QUrl filename)
{
  return writeYaml(filename, POD{.zegrapher = SOFTWARE_VERSION,
                          .math_objects = zg::mathWorld->exportPod(),
                          .graph = graphSettings.exportPod()});
}

void Information::saveAs(QUrl filename)
{
  if (exportYaml(filename) and filename != documentFile)
  {
    documentFile = filename;
    emit documentFileChanged();
  }
}

void Information::save()
{
  if (not documentFile.isEmpty())
    exportYaml(documentFile);
}

QString Information::getDocumentName() const
{
  return documentFile.fileName();
}

QUrl Information::getFolder() const
{
  if (not documentFile.isEmpty())
    return documentFile.adjusted(QUrl::RemoveFilename);

  return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
}

template <class Pod>
bool Information::writeYaml(QUrl filename, const Pod& pod)
{
  auto exp_content = glz::write_yaml(pod);
  if (not exp_content)
  {
    appendIoErr(
      {.title = tr("Internal error"),
       //: %1 is the email address of the author, and %2 the page of the issues on GitHub
       .text = tr("Please report this bug to %1 or %2, with a way to reproduce it.")
                 .arg(CONTACT_ADDRESS, REPOSITORY_URL "/issues"),
       .details = tr("Could not serialize ZeGrapher's state")});
    return false;
  }

  const QString path = filename.toLocalFile();
  if (path.isEmpty())
  {
    appendIoErr({
      .title = tr("Could not save the document"),
      .file = filename.toString(),
      .text = tr("The target file path is invalid."),
    });
    return false;
  }

  QFileInfo info(path);
  if (not info.absoluteDir().mkpath("."))
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("Could not create the folder."),
                 .details = info.absolutePath()});
    return false;
  }

  QSaveFile file(path);
  if (not file.open(QIODevice::WriteOnly))
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("Could not open the file for writing."),
                 .details = file.errorString()});
    return false;
  }

  qint64 written = file.write(exp_content->data(), exp_content->size());
  if (written != qint64(exp_content->size()))
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("The write did not finish. The file is left unchanged."),
                 .details = file.errorString()});
    return false;
  }

  if (not file.commit())
  {
    appendIoErr({.title = tr("Could not save the document"),
                 .file = path,
                 .text = tr("Could not replace the file with the new version."),
                 .details = file.errorString()});
    return false;
  }

  return true;
}

void Information::importYaml(QUrl filename)
{
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

  // documents from ZeGrapher 4.0.0_beta3 or older hold the app settings under
  // 'app', which this read skips
  POD pod;
  const auto read_error = glz::read_yaml<zg::yml::skip_unknown_keys>(pod, content);
  const std::optional<QString> writer = newerWriter(content);

  if (read_error)
  {
    appendIoErr({.title = tr("Could not load the document"),
                 .file = path,
                 .text = writer
                   //: %1 and %2 are versions of ZeGrapher, %3 the address of its website
                   ? tr("ZeGrapher %1 wrote this document, and this is version %2. Get "
                        "the latest version from %3 to open it.")
                       .arg(*writer, SOFTWARE_VERSION, HOMEPAGE_URL)
                   : tr("The file is not a valid ZeGrapher document."),
                 .details = QString::fromStdString(glz::format_error(read_error, content))});
    return;
  }

  graphSettings.importPod(std::move(pod.graph).value_or(ZeGraphSettings::POD{}));
  if (pod.math_objects) zg::mathWorld->importPod(std::move(*pod.math_objects));

  // the read skips unknown keys, which a newer version can write
  if (writer)
    appendIoErr({.title = tr("Document from a newer version"),
                 .file = path,
                 //: %1 and %2 are versions of ZeGrapher, %3 the address of its website
                 .text = tr("ZeGrapher %1 wrote this document, and this is version %2. Get "
                            "the latest version from %3 to open all of it.")
                           .arg(*writer, SOFTWARE_VERSION, HOMEPAGE_URL)});
}
