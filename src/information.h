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

#include "Calculus/function.h"
#include "Calculus/regression.h"
#include "Calculus/seqcalculator.h"
#include "MathObjects/mathobject.h"
#include "Widgets/pareqwidget.h"

#include <QtQml/qqmlregistration.h>

#include <zecalculator/zecalculator.h>

namespace zg {
  struct PlotStyle;
}

class Information: public QObject
{
  Q_OBJECT

  Q_PROPERTY(
    ZeAppSettings appSettings READ getAppSettings WRITE setAppSettings NOTIFY appSettingsChanged)
  Q_PROPERTY(GraphRange range READ getGraphRange WRITE setGraphRange NOTIFY graphRangeChanged)
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
  const GraphRange& getGraphRange() const;
  const ZeAppSettings& getAppSettings() const;

  void addDataList(const std::shared_ptr<const UserData>& userData);
  void removeDataList(const std::shared_ptr<const UserData>& userData);

  int getDataListsCount();
  std::shared_ptr<const UserData> getDataPoints(int index);

  QPalette getValidSyntaxPalette() const;
  QPalette getInvalidSyntaxPalette() const;

  void addDataRegression(Regression* reg);
  void removeDataRegression(Regression* reg);
  Regression* getRegression(int index);
  QList<Regression*> getRegressions();
  int getRegressionsCount();

  void setParEqsListPointer(QList<ParEqWidget*>* list);
  QList<ParEqWidget*>* getParEqsList();

  void checkParametricEquations();

  void setSequencesList(QList<SeqCalculator*> list);
  QList<SeqCalculator*> getSeqsList();

  void setFunctionsList(QList<Function*> list);
  QList<Function*> getFuncsList();

  void setExportFileName(QString fileName);
  QString getExportFileName();

  const zc::rpn::MathWorld& getMathWorld() const { return mathWorld; }
  zc::rpn::MathWorld& getMathWorld() { return mathWorld; }

  void registerMathObject(zg::MathObject*);
  void deregisterMathObject(zg::MathObject*);

  const std::vector<zg::MathObject*>& getMathObjects() const { return mathObjects; }

  void mathObjectUpdated();

signals:
  void newOrthonormalityState(bool orth);
  void graphRangeChanged(GraphRange range);
  void styleUpdated();
  void dataUpdated();
  void updateOccured();
  void drawStateUpdateOccured();
  void animationUpdate();
  void regressionAdded(Regression* reg);
  void regressionRemoved(Regression* reg);
  void viewSettingsChanged();
  void axesSettingsChanged();
  void gridSettingsChanged();
  void graphSizeSettingsChanged();
  void graphZoomSettingsChanged();
  void estheticSettingsChanged();
  void appSettingsChanged();
  void mathObjectsChanged(QStringList objectNames);

public slots:
  void emitUpdateSignal();
  void emitDataUpdate();
  void emitDrawStateUpdate();
  void emitAnimationUpdate();

  void setGraphRange(const GraphRange& range);
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
  std::list<std::shared_ptr<UserData const>> userDataSets;

  QList<Regression*> regressions;

  QList<Function*> functions;
  QList<SeqCalculator*> sequences;

  GraphRange range;
  ZeZoomSettings zoomSettings;
  ZeSizeSettings sizeSettings;
  ZeAxesSettings axesSettings;
  ZeGridSettings gridSettings;
  ZeGraphSettings graphSettings;
  ZeAppSettings appSettings;
  QString exportFileName;
  zc::MathWorld<zc_t> mathWorld;

  std::vector<zg::MathObject*> mathObjects;

  QList<ParEqWidget*>* parEqWidgets;
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
