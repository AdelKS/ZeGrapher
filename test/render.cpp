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
#include "initresources.h"
#include "utils.h"
#include "GraphDraw/interactivegraph.h"

#include <QQuickWindow>
#include <QTest>

#include <cstdio>

/// @brief draws a line and markers with the platform's scene graph backend:
///        OpenGL, Direct3D 11 or Metal
/// @note shows a window, so it runs only when ZG_RENDER_TEST is 1
int main()
{
  // meson counts the exit code 77 as a skipped test
  constexpr int skipped = 77;

  if (qEnvironmentVariableIntValue("ZG_RENDER_TEST") != 1)
    return skipped;

  initResources();

  // the messages go to stderr, which is unbuffered, so a test that meson kills
  // on timeout still shows how far it got. QSG_INFO prints the RHI backend
  qputenv("QSG_INFO", "1");
  TestApp app(TestApp::Native);

  test(app.load(R"(graph:
  range: {x: {min: "-1", max: "4"}, y: {min: "-1", max: "1"}}
  background: {dark: "#ffffff", light: "#ffffff"}
  grid: {x: {show: false}, y: {show: false}}
  subgrid: {x: {show: false}, y: {show: false}}
math_objects:
  - type: equation
    equation: f(x) = x / 10 + 0.3
    style: {color: {dark: "#ff0000", light: "#ff0000"}, line_width: 4}
  - type: equation
    equation: u(n) = -0.5 ; u(n - 1)
    end: "3"
    style: {color: {dark: "#0000ff", light: "#0000ff"}, point_style: disc, point_width: 6}
)"));

  QQuickWindow window;
  window.resize(500, 300);

  bool failed = false;
  QObject::connect(&window, &QQuickWindow::sceneGraphError, [&](QQuickWindow::SceneGraphError, const QString& message) {
    std::fprintf(stderr, "no scene graph: %s\n", qPrintable(message));
    failed = true;
  });

  InteractiveGraph graph(window.contentItem());
  graph.setSize(QSizeF(500, 300));

  window.show();
  if (not QTest::qWaitForWindowExposed(&window) or failed)
    return skipped;

  std::fprintf(stderr, "window exposed, grabbing a frame\n");
  const QImage frame = window.grabWindow();
  test(not frame.isNull());

  std::fprintf(stderr, "scene graph backend: %d\n", int(window.rendererInterface()->graphicsApi()));

  int red = 0, blue = 0;
  for (int y = 0; y != frame.height(); y++)
    for (int x = 0; x != frame.width(); x++)
    {
      const QColor c = frame.pixelColor(x, y);
      red += c.red() > 200 and c.green() < 80 and c.blue() < 80;
      blue += c.blue() > 200 and c.red() < 80 and c.green() < 80;
    }

  std::fprintf(stderr, "red pixels: %d, blue pixels: %d\n", red, blue);

  // a line across the graph, and four discs
  test(red > 500);
  test(blue > 4 * 50);

  return 0;
}
