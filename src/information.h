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
  Q_PROPERTY(zg::GraphRange* range READ getGraphRange)
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
  Q_INVOKABLE zg::GraphRange* getGraphRange() { return graph_range; }

  /// @brief graph range change from user mouse interaction
  void setGraphRangeMouseEdit(const zg::real_range2d&);

  QPalette getValidSyntaxPalette() const;
  QPalette getInvalidSyntaxPalette() const;

  void setExportFileName(QString fileName);
  QString getExportFileName();

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
  void rangeChangedWithMouse();

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
  zg::GraphRange* graph_range = nullptr;
};

#endif // INFORMATION_H
