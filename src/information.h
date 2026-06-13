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

///@note must be instanced after QGuiApplication (instanced in main.cpp)
class Information: public QObject
{
  Q_OBJECT
  QML_ANONYMOUS

  Q_PROPERTY(ZeAppSettings* appSettings READ getAppSettingsPtr CONSTANT)
  Q_PROPERTY(ZeGraphSettings* graphSettings READ getGraphSettingsPtr CONSTANT)
  Q_PROPERTY(double pixelDensity READ getPixelDensity NOTIFY pixelDensityChanged)

public:
  Information(QObject* parent = nullptr);

  ZeAppSettings* getAppSettingsPtr() { return &appSettings; }
  double getPixelDensity() const { return pixelDensity; }
  ZeGraphSettings* getGraphSettingsPtr() { return &graphSettings; }

signals:
  void appSettingsChanged();
  void pixelDensityChanged();
  void availableSheetSizePxChanged();
  void availableSheetSizeCmChanged();
  void graphSettingsChanged();

public slots:
  void screenChanged(QWindow*);

  QString exportYaml(QUrl filename);
  QString importYaml(QUrl filename);

public:
  zc::eval::Cache mathObjectCache;
  ZeAppSettings appSettings;
  ZeGraphSettings graphSettings;

  struct POD {
    std::optional<zg::MathWorld::POD> math_objects;
    std::optional<ZeGraphSettings::POD> graph;
    std::optional<ZeAppSettings::POD> app;
  };

  struct PartialGraphPOD {
    std::optional<ZeGraphSettings::POD> graph;
  };

  struct PartialAppPOD {
    std::optional<ZeAppSettings::POD> app;
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

///@note defined in the main() to come after QGuiApplication
inline Information* information = nullptr;

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
    // pointer should be non nullptr by now
    Q_ASSERT(information);
    // The engine has to have the same thread affinity as the singleton.
    Q_ASSERT(engine->thread() == information->thread());

    // There can only be one engine accessing the singleton.
    if (s_engine)
      Q_ASSERT(engine == s_engine);
    else
      s_engine = engine;

    // Explicitly specify C++ ownership so that the engine doesn't delete
    // the instance.
    QJSEngine::setObjectOwnership(information,
                                  QJSEngine::CppOwnership);
    return information;
  }

private:
  inline static QJSEngine *s_engine = nullptr;
};
