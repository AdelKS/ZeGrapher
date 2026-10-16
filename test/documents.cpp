/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "app.h"
#include "utils.h"

#include <QDir>

namespace {

QByteArray readFile(const QString& path)
{
  QFile file(path);
  test(file.open(QIODevice::ReadOnly));
  return file.readAll();
}

/// @brief loads a document in a fresh app start, and saves it
/// @returns what the app saved
QByteArray roundTrip(TestApp& app, const QString& from, const QString& to)
{
  app.load(readFile(from));
  test(information->getIoErrorCount() == 0);

  information->exportYaml(QUrl::fromLocalFile(to));
  test(information->getIoErrorCount() == 0);

  return readFile(to);
}

zg::MathObject& object(size_t i)
{
  return *zg::mathWorld->getMathObjects().at(i);
}

}

/// @param argv[1]  the website/scripts/make-images documents folder
int main(int argc, char* argv[])
{
  test(argc == 2);

  TestApp app;

  // a document saves the way it loads: saving it again changes nothing
  const QFileInfoList documents = QDir(argv[1]).entryInfoList({"*.zg"}, QDir::Files);
  test(not documents.isEmpty());

  for (const QFileInfo& document: documents)
  {
    const QString once = app.path(document.baseName() + "-once.zg");
    const QString twice = app.path(document.baseName() + "-twice.zg");
    const QByteArray saved = roundTrip(app, document.filePath(), once);
    test(roundTrip(app, once, twice) == saved);
  }

  // a sequence that references a later object keeps its plot style
  test(app.load(R"(math_objects:
  - type: equation
    equation: u(n) = 0 ; v(n) + 1
    style: {line_style: solid, point_style: disc}
  - type: equation
    equation: v(n) = 0 ; n
)"));
  test(object(0).getStyle()->lineStyle == zg::PlotStyle::Solid);
  test(object(0).getStyle()->pointStyle == zg::PlotStyle::Disc);

  // the bounds that set_value() gives a constant aren't saved
  test(app.load(R"(math_objects:
  - type: constant
    name: a
    value: 3
)"));
  test(object(0).getConstant()->getFrom() == 1.5);
  test(object(0).getConstant()->getTo() == 4.5);
  test(not object(0).getConstant()->exportPod().from);
  test(not object(0).getConstant()->exportPod().to);

  // a sheet without a positive size fills the window
  test(app.load(R"(graph:
  size: {width: 0, height: 10}
)"));
  test(information->graphSettings.getSize().sheetFillsWindow);

  // an equation's range doesn't clash with user object names
  test(app.load(R"(math_objects:
  - type: equation
    equation: f(x) = cos(x)
  - type: constant
    name: start
    value: 1
  - type: constant
    name: end
    value: 2
)"));
  test(object(1).isValid());
  test(object(2).isValid());

  // a document can't set a value outside the panel's range
  test(app.load(R"(graph:
  min_points: 30
  max_points: 70
  zoom: -3
  size: {scaling: .nan, width: 1e9, height: 5}
  axes:
    line_width: 0
    x: {tick_spacing: 1000000000, max_digits: 0}
  subgrid:
    y: {subdivs: 100000}
math_objects:
  - type: constant
    steps: 1000000000
)"));
  {
    const ZeGraphSettings& s = information->graphSettings;
    test(s.getMaxPointsLg2() == 16 and s.getMinPointsLg2() == 16);
    test(s.getZoom().zoom == ZeZoomSettings::minZoom);
    test(s.getSize().scalingFactor == ZeSizeSettings::minScaling);
    test(s.getSize().cmSheetSize == QSizeF(ZeSizeSettings::maxCm, 5));
    test(s.getAxes().lineWidth == ZeAxesSettings::minLineWidth);
    test(s.getAxes().x.tickRelSpacing == ZeAxisSettings::maxTickSpacing);
    test(s.getAxes().x.linSettings.maxDigitsNum == ZeAxisSettings::minDigits);
    test(s.getSubgrid().y.subdivs == Ze1DSubgridSettings::maxSubdivs);
    test(object(0).getConstant()->getSteps() == 100);
  }

  // a document from a newer version loads, and the app suggests updating
  test(not app.load(R"(zegrapher: 99.0.0
math_objects:
  - type: equation
    equation: f(x) = x
    shiny: true
)"));
  test(zg::mathWorld->getMathObjects().size() == 1);
  test(information->popIoError().text.contains("99.0.0"));

  // and when it can't load, the app suggests updating too
  test(not app.load(R"(zegrapher: 99.0.0
math_objects:
  - type: equation
    equation: f(x) = x
    style: {point_style: star}
)"));
  test(zg::mathWorld->getMathObjects().empty());
  test(information->popIoError().text.contains("99.0.0"));
  test(information->getIoErrorCount() == 0);

  return 0;
}
