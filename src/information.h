/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#include "Calculus/regression.h"
#include "Widgets/pareqwidget.h"
#include "Calculus/seqcalculator.h"
#include "Calculus/function.h"
#include "Widgets/straightlinewidget.h"
#include "Widgets/tangentwidget.h"

#include <QtQml/qqmlregistration.h>

#include <zecalculator/zecalculator.h>

/// @brief type of math objects to use in ZeCalculator
inline constexpr zc::parsing::Type zc_t = zc::parsing::Type::RPN;

class Information: public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(ZeAppSettings appSettings READ getAppSettings WRITE setAppSettings NOTIFY appSettingsChanged)
    Q_PROPERTY(GraphRange range READ getGraphRange WRITE setGraphRange NOTIFY graphRangeChanged)

public:
    Information();

    const ZeZoomSettings& getGraphZoomSettings() const;
    const ZeSizeSettings& getGraphSizeSettings() const;
    const ZeGraphSettings& getGraphSettings() const;
    const ZeGridSettings& getGridSettings() const;
    const ZeAxesSettings& getAxesSettings() const;
    const ZeGraphSettings &getEstheticSettings() const;
    const GraphRange& getGraphRange() const;
    const ZeAppSettings& getAppSettings() const;

    void addDataList(const std::shared_ptr<const UserData> &userData);
    void removeDataList(const std::shared_ptr<const UserData> &userData);

    int getDataListsCount();
    std::shared_ptr<const UserData> getDataPoints(int index);

    QPalette getValidSyntaxPalette() const;
    QPalette getInvalidSyntaxPalette() const;

    void addDataRegression(Regression *reg);
    void removeDataRegression(Regression *reg);
    Regression* getRegression(int index);
    QList<Regression*> getRegressions();
    int getRegressionsCount();

    void setParEqsListPointer(QList<ParEqWidget*> *list);
    QList<ParEqWidget*>* getParEqsList();

    void setTangentsListPointer(QList<TangentWidget*> *list);
    QList<TangentWidget*>* getTangentsList();

    void setStraightLinesListPointer(QList<StraightLineWidget*> *list);
    QList<StraightLineWidget*>* getStraightLinesList();

    void checkParametricEquations();

    void setSequencesList(QList<SeqCalculator*> list);
    QList<SeqCalculator*> getSeqsList();

    void setFunctionsList(QList<Function*> list);
    QList<Function*> getFuncsList();

    void setExportFileName(QString fileName);
    QString getExportFileName();

    const zc::rpn::MathWorld& getMathWorld() const { return mathWorld; }

signals:
    void newOrthonormalityState(bool orth);
    void graphRangeChanged(GraphRange range);

    void dataUpdated();
    void updateOccured();
    void drawStateUpdateOccured();
    void animationUpdate();
    void regressionAdded(Regression *reg);
    void regressionRemoved(Regression *reg);
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

    void setGraphRange(const GraphRange &range);
    void setOrthonormal(bool state);
    void setGraphSizeSettings(const ZeSizeSettings &graphSizeSettings);
    void setGraphZoomSettings(const ZeZoomSettings &zoomSettings);
    void setAxesSettings(const ZeAxesSettings &axesSettings);
    void setGridSettings(const ZeGridSettings &gridSettings);
    void setGraphSettings(const ZeGraphSettings &graphSettings);
    void setAppSettings(const ZeAppSettings& appSettings);

protected:

    std::list<std::shared_ptr<UserData const>> userDataSets;

    QList<Regression*> regressions;

    QList<TangentWidget*> *tangents;
    QList<StraightLineWidget*> *lines;

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

    QList<ParEqWidget*> *parEqWidgets;
};

inline Information information;

#endif // INFORMATION_H
