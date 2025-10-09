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

#ifndef INFORMATION_H
#define INFORMATION_H

#include <QtQml/qqmlregistration.h>

#include <zecalculator/zecalculator.h>

#include "Utils/graphrange.h"

namespace zg {
  struct PlotStyle;
}

class Information: public QObject
{
  Q_OBJECT

  Q_PROPERTY(
    ZeAppSettings appSettings READ getAppSettings WRITE setAppSettings NOTIFY appSettingsChanged)
  Q_PROPERTY(zg::GraphRange* range MEMBER graph_range)
  Q_PROPERTY(ZeZoomSettings graphZoomSettings READ getGraphZoomSettings WRITE setGraphZoomSettings
               NOTIFY graphZoomSettingsChanged)
  Q_PROPERTY(ZeSizeSettings graphSizeSettings READ getGraphSizeSettings WRITE setGraphSizeSettings
               NOTIFY graphSizeSettingsChanged)

public:
  Information(QObject* parent = nullptr);

  const ZeZoomSettings& getGraphZoomSettings() const;
  const ZeSizeSettings& getGraphSizeSettings() const;
  const ZeGraphSettings& getGraphSettings() const;
  const ZeGridSettings& getGridSettings() const;
  const ZeAxesSettings& getAxesSettings() const;
  const ZeGraphSettings& getEstheticSettings() const;
  const ZeAppSettings& getAppSettings() const;

  QPalette getValidSyntaxPalette() const;
  QPalette getInvalidSyntaxPalette() const;

  void setExportFileName(QString fileName);
  QString getExportFileName();

  zg::GraphRange* graph_range = nullptr;

signals:
  void newOrthonormalityState(bool orth);
  void styleUpdated();
  void dataUpdated();
  void updateOccured();
  void drawStateUpdateOccured();
  void animationUpdate();
  void viewSettingsChanged();
  void axesSettingsChanged();
  void gridSettingsChanged();
  void graphSizeSettingsChanged();
  void graphZoomSettingsChanged();
  void estheticSettingsChanged();
  void appSettingsChanged();

public slots:
  void emitUpdateSignal();
  void emitDataUpdate();
  void emitDrawStateUpdate();
  void emitAnimationUpdate();

  void setOrthonormal(bool state);
  void setGraphSizeSettings(const ZeSizeSettings& graphSizeSettings);
  void setGraphZoomSettings(const ZeZoomSettings& zoomSettings);
  void setAxesSettings(const ZeAxesSettings& axesSettings);
  void setGridSettings(const ZeGridSettings& gridSettings);
  void setGraphSettings(const ZeGraphSettings& graphSettings);
  void setAppSettings(const ZeAppSettings& appSettings);

public:
  zc::eval::Cache mathObjectCache;

protected:
  ZeZoomSettings zoomSettings;
  ZeSizeSettings sizeSettings;
  ZeAxesSettings axesSettings;
  ZeGridSettings gridSettings;
  ZeGraphSettings graphSettings;
  ZeAppSettings appSettings;
  QString exportFileName;
};

inline Information information;

/// @brief register the 'information' global variable with QML
/// @note  see https://doc.qt.io/qt-6/qml-singleton.html#exposing-an-existing-object-as-a-singleton

struct InformationForeign
{
  Q_GADGET
  QML_FOREIGN(Information)
  QML_SINGLETON
  QML_NAMED_ELEMENT(Information)

public:

  static Information *create(QQmlEngine *, QJSEngine *engine)
  {
    // The engine has to have the same thread affinity as the singleton.
    Q_ASSERT(engine->thread() == information.thread());

    // There can only be one engine accessing the singleton.
    if (s_engine)
      Q_ASSERT(engine == s_engine);
    else
      s_engine = engine;

    // Explicitly specify C++ ownership so that the engine doesn't delete
    // the instance.
    QJSEngine::setObjectOwnership(&information,
                                  QJSEngine::CppOwnership);
    return &information;
  }

private:
  inline static QJSEngine *s_engine = nullptr;
};

#endif // INFORMATION_H
