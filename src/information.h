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

#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include <zecalculator/zecalculator.h>

#include "MathObjects/mathworld.h"
#include "Utils/appsettings.h"
#include "Utils/graphsettings.h"

namespace zg {
  struct PlotStyle;
}

class Information: public QObject
{
  Q_OBJECT
  QML_ANONYMOUS

  Q_PROPERTY(ZeAppSettings* appSettings READ getAppSettings NOTIFY appSettingsChanged)
  Q_PROPERTY(ZeGraphSettings* graphSettings MEMBER graphSettings NOTIFY graphSettingsChanged)
  Q_PROPERTY(double pixelDensity READ getPixelDensity NOTIFY pixelDensityChanged)

public:
  Information(QObject* parent = nullptr);

  ZeAppSettings* getAppSettings() { return &appSettings; }
  Q_INVOKABLE void setAppFont(QFont);
  double getPixelDensity() const { return pixelDensity; }

  void setExportFileName(QString fileName);
  QString getExportFileName();

signals:
  void newOrthonormalityState(bool orth);
  void styleUpdated();
  void dataUpdated();
  void updateOccured();
  void drawStateUpdateOccured();
  void animationUpdate();
  void appFontChanged();
  void appSettingsChanged();
  void pixelDensityChanged();
  void availableSheetSizePxChanged();
  void availableSheetSizeCmChanged();
  void graphSettingsChanged();

public slots:
  void emitUpdateSignal();
  void emitDataUpdate();
  void emitDrawStateUpdate();
  void emitAnimationUpdate();

  void setOrthonormal(bool state);
  void screenChanged(QWindow*);

  QString exportYaml(QUrl filename);
  QString importYaml(QUrl filename);

public:
  zc::eval::Cache mathObjectCache;
  ZeAppSettings appSettings;
  ZeGraphSettings* graphSettings = nullptr;

  struct POD {
    std::optional<zg::MathWorld::POD> math_objects;
    std::optional<ZeGraphSettings::POD> graph;
  };

  struct PartialGraphPOD {
    std::optional<ZeGraphSettings::POD> graph;
  };

  struct PartialMathPOD {
    std::optional<zg::MathWorld::POD> math_objects;
  };

protected:
  QString exportFileName;

  /// @note in pixels per centimeter
  double pixelDensity = 50;

  QSize availableSheetSizePx;
  QSizeF availableSheetSizeCm;
};
